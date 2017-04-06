#ifndef RX_TILESETLOADER
#define RX_TILESETLOADER

#include <misc/RXHashmap.h>
#include <rxmapbase/RXMapEnum.h>

class RXFileManager;
class RXTileset;

class RXTilesetLoader
{
public:
	RXTilesetLoader(RXFileManager *ressource);
	~RXTilesetLoader(void);

	const RXTileset * getTileset(RXMapEnum::TileSet tile);

private:
	RXFileManager *ressource;
	RXTPtrHash<RXMapEnum::TileSet, RXTileset> tileset;
//	RXTPtrHash<int, RXTileset> tileset;


};

#endif
