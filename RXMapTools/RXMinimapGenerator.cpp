#include "stdafx.h"
#include <rxmaptools/RXMinimapGenerator.h>

#include <comutil.h>

#include <rxfile/RXMPQReader.h>
#include <rxfile/RXSCMpqReader.h>

#include <rxmaptools/RXMinimap.h>
#include <rxmapbase/RXRepMap.h>
#include <rxmapbase/RXCHKMap.h>
#include <rxmaptools/RXHQMapConverter.h>
#include <rxmaptools/RXLQMapConverter.h>
#include <rxmaptools/RXRenderableMap.h>
#include <rxmaptools/RXBitmap.h>

#include <BWlib/BWrepAPI.h>
#include <BWTools/RXReplay.h>
#include <rxcache/RXCache.h>

RXMinimapGenerator::RXMinimapGenerator(void)
{
}

RXMinimapGenerator::~RXMinimapGenerator(void)
{
}





RXBitmap * RXMinimapGenerator::generateRatio(const std::wstring &path, int ratio, bool highQuality, int render /* flags from RXMinimap::enum*/)
{
    //try to load the map
    const RXReplay *replay;
    std::auto_ptr<RXMap> map(load(path, replay));

    int width = getMaxDimensionFromRatio(map.get(), (float)ratio);

    return generate(map.get(), replay, width, highQuality, render);
}



RXBitmap * RXMinimapGenerator::generateFixedWidth (const std::wstring &path, int width, bool highQuality, int render /* flags from RXMinimap::enum*/)
{


    //try to load the map
    const RXReplay *replay;
    std::auto_ptr<RXMap> map(load(path, replay));

    return generate(map.get(), replay, width, highQuality, render);
}



RXBitmap * RXMinimapGenerator::generate(const RXMap *map, const RXReplay *replay, int width, bool highQuality, int render)
{
    //Init
    initRendering();

    //Generate a renderable version of the map
    std::auto_ptr<RXRenderableMap> renderMap(convert(map, replay,highQuality));

    //generate the minimap
    return minimapGen->generate(renderMap.get(),getValidMaxDimension(width), render);
}
















void RXMinimapGenerator::initRendering()
{
    if (mpqReader.get() == NULL)
        mpqReader.reset(new RXSCMpqReader(RXSCMpqReader::USE_ALL));

    if (minimapGen.get() == NULL)
        minimapGen.reset(new RXMinimap(mpqReader.get()));
}


RXRenderableMap *RXMinimapGenerator::convert(const RXMap *map, const RXReplay *replay, bool hq)
{
    //Set up the converter
    std::auto_ptr<RXMapConverter> converter;

    if (hq == false)
        converter.reset(new RXLQMapConverter());
    else
        converter.reset(new RXHQMapConverter(mpqReader.get()));

    //Create the renderablemap
    return converter->convert(map, replay);
}





//Get a RXMap from a file, be it .scm or .rep
RXMap * RXMinimapGenerator::load(const std::wstring &path, const RXReplay *&out)
{
    out = NULL;

    RXMap *map = loadSCM(path);
    if (map)
        return map;

    map= loadRep(path,out);

    if (map)
        return map;
    throw std::runtime_error("Not a valid .scx/.scm/.rep file");
}



RXMap * RXMinimapGenerator::loadSCM(const std::wstring &path)
{
    try
    {
        RXMPQReader scmReader(path);
        return new RXCHKMap(scmReader.getAutoFile("staredit\\scenario.chk").get());
    }
    catch(const std::runtime_error &)
    {
        return NULL;
    }
}

RXMap * RXMinimapGenerator::loadRep(const std::wstring &path, const RXReplay *&out)
{
    try
    {
        std::string path2 = (LPCSTR)(_bstr_t)path.c_str();


        BWrepFile file;
        if (file.Load(path2.c_str()) != 0)
            return NULL;

        if (replayCache.get() == NULL)
            replayCache.reset(new RXCache);

        out = replayCache->getReplay(path.c_str());
        return new RXRepMap(file);
    }
    catch(const std::runtime_error &)
    {
        return NULL;
    }
}



int RXMinimapGenerator::getMaxDimensionFromRatio(const RXMap *map, float ratio)
{
    int destSize = map->getWidth();

    if (destSize<map->getHeight())
        destSize = map->getHeight();

    destSize = (int)( (float)(32*destSize) / ratio);

    if (destSize < 16)
        destSize = 16;

    return destSize;
}


int RXMinimapGenerator::getValidMaxDimension(int maxDim)
{
    if (maxDim < 16)
        maxDim = 16;

    return maxDim;
}