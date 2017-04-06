#include <iostream>
#include <sstream>
#include <string>
#include <deque>
#include <fstream>
#include <stdexcept>
#include <rxmaptools/RXMinimap.h>
#include <rxfile/RXMPQReader.h>


#include <rxmapbase/RXRepMap.h>
#include <rxmapbase/RXCHKMap.h>
#include <rxmaptools/RXHQMapConverter.h>
#include <rxmaptools/RXLQMapConverter.h>

#include <rxmaptools/RXRenderableMap.h>
#include <rxfile/RXSCMpqReader.h>
#include <rxmaptools/RXBitmap.h>
#include <memory>

#include <BWlib/BWrepAPI.h>
#include <BWTools/RXReplay.h>

#include "repasmmap.h"


static void generate(const char *datPath,const char *srcPath, const char *destPath, 
					 RXMap *map, int quality, int maxDimension, int jpegQuality, int renderingMode, const RXReplay *replay= NULL)
{

		std::auto_ptr<RXMapConverter> converter;
		RXSCMpqReader reader(RXSCMpqReader::USE_ALL, datPath);
		
		if (quality == 0)
			converter.reset(new RXLQMapConverter());
		else
			converter.reset(new RXHQMapConverter(&reader));


		//Create the renderablemap
		std::auto_ptr<RXRenderableMap> renderMap(converter->convert(map, replay));

		//Generate it
		RXMinimap minimap(&reader);

		//generate the minimap
		std::auto_ptr<RXBitmap> dest(minimap.generate(renderMap.get(),maxDimension, renderingMode));

#ifndef RX_NO_JPEG
		dest->saveToJPEG(destPath,jpegQuality);
#else
		dest->saveToBMP(destPath);
#endif
}



static RXMap * loadRep(const char *path, RXReplay *&out)
{
	BWrepFile file;
	if (file.Load(path) != 0)
		throw std::runtime_error("Invalid replay file");

	out = new RXReplay(file);

	return new RXRepMap(file);
}


static int getMaxDimensionFromRatio(const RXMap *map, float ratio)
{
	int destSize = map->getWidth();

	if (destSize<map->getHeight())
		destSize = map->getHeight();

	destSize = (int)( (float)(32*destSize) / ratio);

	if (destSize < 16)
		destSize = 16;

	return destSize;
}


static int getValidMaxDimension(int maxDim)
{
	if (maxDim < 16)
		maxDim = 16;

	return maxDim;
}




RXMap * getCHKMap(const char *path)
{
	RXMPQReader scmReader(path);
	return new RXCHKMap(scmReader.getAutoFile("staredit\\scenario.chk").get());
}


void generateMinimapDimFromScm(const char *datPath,const char *srcPath, const char *destPath, int maxDimension, int quality, int jpegQuality, int renderingMode)
{
	std::auto_ptr<RXMap> map(getCHKMap(srcPath));
	generate(datPath, srcPath, destPath, map.get(), quality, getValidMaxDimension(maxDimension),jpegQuality, renderingMode);
}



void generateMinimapRatioFromScm(const char *datPath,const char *srcPath, const char *destPath, float ratio, int quality, int jpegQuality, int renderingMode)
{
	std::auto_ptr<RXMap> map(getCHKMap(srcPath));

	RXMap *pMap = map.get();

	int destSize = getMaxDimensionFromRatio(pMap, ratio);
	generate(datPath, srcPath, destPath, pMap, quality, destSize, jpegQuality, renderingMode);
}











void generateMinimapDimFromRep(const char *datPath,const char *srcPath, const char *destPath, int maxDimension, int quality, int jpegQuality, int renderingMode)
{

	RXReplay *rep;
	std::auto_ptr<RXReplay> replay;
	std::auto_ptr<RXMap> map(loadRep(srcPath,rep));
	replay.reset(rep);

	generate(datPath, srcPath, destPath, map.get(), quality, getValidMaxDimension(maxDimension),jpegQuality, renderingMode, rep);

}



void generateMinimapRatioFromRep(const char *datPath,const char *srcPath, const char *destPath, float ratio, int quality, int jpegQuality, int renderingMode)
{
	RXReplay *rep;
	std::auto_ptr<RXReplay> replay;
	std::auto_ptr<RXMap> map(loadRep(srcPath,rep));
	replay.reset(rep);

	RXMap *pMap = map.get();

	int destSize = getMaxDimensionFromRatio(pMap, ratio);
	generate(datPath, srcPath, destPath, pMap, quality, destSize,jpegQuality, renderingMode, rep);
}







