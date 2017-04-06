#include "stdafx.h"
#include <sstream>
#include <rxmaptools/RXTileset.h>
#include <rxfile/RXFileManager.h>

RXTileset::RXTileset(RXFileManager *fm, RXMapEnum::TileSet tileSet)
{

	std::string name = nameFromTileset(tileSet);
	load(fm,"tileset",nameFromTileset(tileSet));

}

RXTileset::~RXTileset(void)
{
}



void RXTileset::load(RXFileManager *fm, const std::string &path, const std::string &baseName)
{
	load<RXCV5>(fm, cv5, path, baseName, "cv5");
	load<RXWPE>(fm, wpe, path, baseName, "wpe");
	load<RXVR4>(fm, vr4, path, baseName, "vr4");
	load<RXVX4>(fm, vx4, path, baseName, "vx4");
}


template <typename T, typename T2>
void RXTileset::load(RXFileManager *fm,T2 &dest,const std::string &path, const std::string &baseName, const std::string &extension)
{
	std::ostringstream str;

	str << path << '\\' << baseName << '.' << extension;

	std::auto_ptr<std::istream> ptr = fm->getAutoFile(str.str());

	dest.reset(new T(ptr.get()));
}




std::string RXTileset::nameFromTileset(RXMapEnum::TileSet tileSet)
{
	using namespace RXMapEnum;

	switch(tileSet)
	{
	case TileSetValue::BADLANDS:		return "badlands"; 
	case TileSetValue::SPACE_PLATFORM:	return "platform"; 
	case TileSetValue::INSTALLATION:	return "install"; 
	case TileSetValue::ASHWORLD:		return "AshWorld"; 
	case TileSetValue::JUNGLE:			return "Jungle"; 
	case TileSetValue::DESERT:			return "Desert"; 
	case TileSetValue::ARTIC:			return "ice"; 
	case TileSetValue::TWILIGHT:		return "twilight"; 

	default:
		return "unknown";
	}
}



























