#ifndef RX_MINIMAP__H
#define RX_MINIMAP__H

#include <memory>
#include <rxmapbase/RXMapEnum.h>

class RXRenderableMap;
class RXBitmap;
class RXFileManager;
class RXTilesetLoader;
class RXReplay;
class RXBitmapCache;
class RXTileset;

class RXMinimap
{
public:
	enum
	{
		RENDER_NOTHING = 0,
		RENDER_DOODADS = 1,
		RENDER_RESOURCES = 2,
		RENDER_STARTING_LOCATIONS = 4,
		RENDER_BUILDINGS = 8,
		RENDER_ALL = RENDER_DOODADS | RENDER_RESOURCES | RENDER_STARTING_LOCATIONS | RENDER_BUILDINGS
	};

	RXMinimap(RXFileManager *ressource);
	~RXMinimap(void);

	RXBitmap * generate(const RXRenderableMap *map, int size, int renderingMode = RENDER_NOTHING, float *outReductionFactor=NULL);
	RXBitmap * getSubsection(const RXRenderableMap *map, const RXReplay *replay, int posX, int posY, int numX, int numY);

private:
	
	RXFileManager *ressource;

	void renderUnit(const RXRenderableMap *map, const RXTileset *ts, RXBitmap *dest, float reductionFactor, int renderingMode, int depX, int depY);
	
	std::auto_ptr<RXTilesetLoader>		tileset;
	std::auto_ptr<RXBitmapCache>		bitmapCache;
};

#endif
