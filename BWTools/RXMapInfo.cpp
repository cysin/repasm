#include "stdafx.h"
#include <sstream>
#include "BWTools/RXMapInfo.h"
#include <rxmapbase/RXMap.h>
#include <rxmapbase/sections/RXMapERASection.h>
#include <rxmapbase/sections/RXMapVERSection.h>
#include <rxmapbase/RXStartingLocation.h>

// When adding/removing/modifying serialized data, change the version here. This will force the repcache files to update (instead of... crashing explorer.exe :p)
static const char * gMapInfoVer = "RXMI04";


RXMapInfo::RXMapInfo(const RXMap *file)
{
    mapName = file->getMapName();
    mapDesc = file->getMapDescription();


	width  = file->getWidth();
	height = file->getHeight();

	isBW = false;
	const RXMapVERSection *sec = file->getVER();
	if(sec)
	{
		if(sec->getVersion() == RXMapEnum::VersionValue::VERSION_BROOD)
			isBW = true;
	}

	buildTileset(file);
    buildFormatedInfo();
	startLocs = RXStartingLocation::listStartingPositions12(file);
}

RXMapInfo::RXMapInfo(std::istream &stream)
{
    std::string sig;

    std::getline(stream, sig, '\0');

    if (sig != gMapInfoVer)
        throw std::runtime_error("Invalid map info");

    std::getline(stream, mapName, '\0');
	std::getline(stream, mapDesc, '\0');

	stream.read((char *)&width,sizeof(width));
	stream.read((char *)&height,sizeof(height));
	stream.read((char *)&tileset,sizeof(tileset));
	stream.read((char *)&isBW,sizeof(isBW));

	// Starting Locations (added in RXMI04)
	size_t nbStartLocs = 0;
	stream.read((char *)&nbStartLocs,sizeof(nbStartLocs));
	startLocs.reserve(nbStartLocs);
	for(size_t i = 0; i < nbStartLocs; i++)
	{
		size_t curLoc = 0;
		stream.read((char *)&curLoc,sizeof(curLoc));
		startLocs.push_back(curLoc);
	}

    buildFormatedInfo();
}

RXMapInfo::~RXMapInfo()
{
}




void RXMapInfo::serialize(std::ostream &str) const 
{
    str << gMapInfoVer << '\0';
    str << mapName <<'\0';
    str << mapDesc <<'\0';

	str.write((const char *)&width,sizeof(width));
	str.write((const char *)&height,sizeof(height));
	str.write((const char *)&tileset,sizeof(tileset));
	str.write((const char *)&isBW,sizeof(isBW));

	// Starting Locations (added in RXMI04)
	size_t nbStartLocs = startLocs.size();
	str.write((const char *)&nbStartLocs,sizeof(nbStartLocs));
	for(size_t i = 0; i < nbStartLocs; i++)
		str.write((const char *)&startLocs[i],sizeof(startLocs[i]));
}


void RXMapInfo::buildFormatedInfo()
{
    buildTilesetName();
    cleanMapName();
    buildDimensions();
}
void RXMapInfo::buildDimensions()
{
	std::ostringstream stream;
	
	stream << width << "x" << height;

	dimensions = stream.str();
}

void RXMapInfo::cleanMapName()
{
	std::ostringstream str;

	for (size_t i=0;i<mapName.size();i++)
	{
		//printf("%i (%x)\n", mapName[i], mapName[i]);
		if (mapName[i] < 0 || mapName[i] >=32)
			str << mapName[i];
	}

	cleanedMapName = str.str();
}

void RXMapInfo::buildTileset(const RXMap *file)
{
    const RXMapERASection * section = file->getERA();
    tileset = section->getTileSet();
}


void RXMapInfo::buildTilesetName()
{
    using namespace RXMapEnum::TileSetValue;

	switch(tileset)
	{
		case BADLANDS:
			tilesetName = "Badlands";
			break;
		case SPACE_PLATFORM:
			tilesetName = "Platform";
			break;
		case INSTALLATION:
			tilesetName = "Install";
			break;
		case ASHWORLD:
			tilesetName = "AshWorld"; 
			break;
		case JUNGLE:
			tilesetName = "Jungle"; 
			break;
		case DESERT:
			tilesetName = "Desert";
			break;
		case ARTIC:
			tilesetName = "Ice";
			break;
		case TWILIGHT:
			tilesetName = "Twilight";
			break;

		default:
			tilesetName = "unknown";
			break;
	}
}
