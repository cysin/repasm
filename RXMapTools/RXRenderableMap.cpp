#include "stdafx.h"
#include <rxmaptools/RXRenderableMap.h>

RXRenderableMap::RXRenderableMap(int width, int height, const unsigned short * tile, RXMapEnum::TileSet tileset)
{

	this->width   = width;
	this->height  = height;
	this->tileset = tileset;

	this->tile = new unsigned short[width * height];
	memcpy(this->tile, tile, width * height * 2);

}

RXRenderableMap::~RXRenderableMap(void)
{
	if (tile)
		delete []tile;
}
