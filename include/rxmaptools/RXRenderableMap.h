#ifndef RX_RENDERABLE_H
#define RX_RENDERABLE_H

#include "RXRenderableUnitContainer.h"
#include <rxmapbase/RXMapEnum.h>


class RXRenderableMap
{
public:
	RXRenderableMap(int width, int height, const unsigned short * tile, RXMapEnum::TileSet tileset);
	~RXRenderableMap(void);

	RXRenderableUnitContainer & getUnit() { return unit;}
	const RXRenderableUnitContainer &getUnit() const { return unit;}


	int getMapWidth()  const { return width;}
	int getMapHeight() const { return height;}

	const unsigned short *getMapTile() const { return tile;}
	RXMapEnum::TileSet getMapTileset() const { return tileset;}

private:

	int width, height;
	unsigned short *tile;
	RXMapEnum::TileSet tileset;

	RXRenderableUnitContainer unit;

};

#endif
