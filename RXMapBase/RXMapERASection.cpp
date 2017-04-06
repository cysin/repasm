#include "stdafx.h"
#include <rxmapbase/sections/RXMapERASection.h>


RXMapERASection::RXMapERASection(std::istream *str, int size) : RXMapSection("ERA ",str, size) 
{
	setTileSet();
}

RXMapERASection::RXMapERASection(const unsigned char *data, int size) : RXMapSection("ERA ",data, size) 
{
	setTileSet();
}

RXMapERASection::~RXMapERASection()
{
}


void RXMapERASection::setTileSet()
{

	unsigned short v = getVal<unsigned short, unsigned short>();
	tileSet = (RXMapEnum::TileSet)(v%8);

}
