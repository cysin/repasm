#include "stdafx.h"
#include <rxmaptools/RXTilesetLoader.h>
#include <rxmaptools/RXTileset.h>


RXTilesetLoader::RXTilesetLoader(RXFileManager *res) : ressource(res)
{
}

RXTilesetLoader::~RXTilesetLoader(void)
{
}



const RXTileset * RXTilesetLoader::getTileset(RXMapEnum::TileSet tile)
{
	RXTPtrHash<RXMapEnum::TileSet, RXTileset>::iterator it = tileset.find(tile);

	if (it != tileset.end())
		return it->second;

	RXTileset *ts = new RXTileset(ressource,tile);
	tileset[tile] = ts;

	return ts;
}
