#include "stdafx.h"

#include <rxmaptools/RXMinimap.h>
#include <rxmaptools/RXTilesetLoader.h>
#include <rxmaptools/RXMapGenerator.h>
#include <rxmapbase/sections/RXMapERASection.h>
#include <rxmaptools/RXRenderableMap.h>
#include <rxmaptools/RXTileset.h>
#include <rxmaptools/RXUnitRenderer.h>
#include <rxmaptools/RXBitmapCache.h>
#include <rxmaptools/RXBitmap.h>
#include <rxmaptools/RXRenderableUnit.h>

class RXUnitRenderer2 : public RXUnitRenderer
{
public:
	RXUnitRenderer2(RXBitmapCache *cache,RXBitmap *dest, float reductionFactor, int depX, int depY)
	{
		this->cache = cache;
		this->dest = dest;
		this->reductionFactor = reductionFactor;
        this->depX = depX;
        this->depY = depY;
	}


	virtual void blendBitmap(int tblID, int frameNumber, RXMapEnum::Palette  palette, int owner, int centerX, int centerY)
	{
		const RXBitmap * btm = cache->getUnitBitmap(tblID,frameNumber, owner, reductionFactor);

		if (btm == NULL) //loading failed, or reduction factor was too strong
			return; 

        centerX-=depX;
        centerY-=depY;

		centerX = (int)((float)centerX / reductionFactor);
		centerY = (int)((float)centerY / reductionFactor);

		int cx = centerX - btm->getWidth()  / 2;
		int cy = centerY - btm->getHeight() / 2;

		btm->blendTo(dest,cx, cy);
	}


	virtual void fillRect(int centerX, int centerY, int width, int height, unsigned int color)
	{

        centerX-=depX;
        centerY-=depY;

		centerX = centerX - width / 2;
		centerY = centerY - height / 2;

		centerX = (int)((float)centerX / reductionFactor);
		centerY = (int)((float)centerY / reductionFactor);

		dest->fillRect(centerX, centerY, (int) (width / reductionFactor), (int) (height / reductionFactor), color);
	}



    int depX;
    int depY;
	float reductionFactor;
	RXBitmap *dest;
	RXBitmapCache *cache;
};













RXMinimap::RXMinimap(RXFileManager *res) :
	ressource(res),
	tileset(new RXTilesetLoader(res))
{
	//ready !
}

RXMinimap::~RXMinimap(void)
{
}




RXBitmap * RXMinimap::generate(const RXRenderableMap *map, int size, int renderingMode, float * outReductionFactor)
{

	//Fetch tileset
	RXMapEnum::TileSet tsValue = map->getMapTileset();
	const RXTileset *ts = tileset->getTileset(tsValue);

	float fFactor;
	RXMapGenerator gen;

	RXBitmap * minimap = gen.generate(map, ts, size,fFactor);


	renderUnit(map, ts, minimap, fFactor, renderingMode,0,0);

	if (outReductionFactor)
		*outReductionFactor = fFactor;

	return minimap;
}


RXBitmap * RXMinimap::getSubsection(const RXRenderableMap *map, const RXReplay *replay, int posX, int posY, int numX, int numY)
{
	//Fetch tileset
	RXMapEnum::TileSet tsValue = map->getMapTileset();
	const RXTileset *ts = tileset->getTileset(tsValue);


	RXMapGenerator gen;
	RXBitmap * minimap = gen.generateSubset(map, ts, posX, posY, numX, numY);

    renderUnit(map, ts, minimap, 1.0f, RENDER_ALL,posX*8, posY*8);
/*
	RXSpriteGenerator::RXAddSprite addSprite = RXSpriteGenerator::ADD_EVERYTHING;

	//If a replay is provided, don't draw starting location
	if (replay)
		addSprite = (RXSpriteGenerator::RXAddSprite) (addSprite & (~RXSpriteGenerator::ADD_STARTINGLOCATION));

	//Add sprite (doodads, ressources...)
	sprite->addToBitmap(map, ts->getPalette(),minimap,addSprite, 1.0f, posX, posY);

	//If a replay is provided, add additionnal info
	if (replay)
		sprite->addPlayerFromReplay(map, replay,ts->getPalette(),minimap,1.0f, posX, posY);
*/
	return minimap;
}






void RXMinimap::renderUnit(const RXRenderableMap *map, const RXTileset *ts,RXBitmap *dest, float reductionFactor, int renderingMode, int depX, int depY)
{
	const RXRenderableUnitContainer &ct =  map->getUnit();

	if (ct.getUnitCount()==0)
		return;


	if (bitmapCache.get() == NULL)
		bitmapCache.reset(new RXBitmapCache(ressource));

	bitmapCache->setActivePalette(ts->getPalette());

	RXUnitRenderer2 renderer(bitmapCache.get(),dest,reductionFactor,depX, depY);

	int target = 0;
	if(renderingMode & RENDER_RESOURCES)
		target |= RXRenderableUnit::RESOURCE;
	if(renderingMode & RENDER_STARTING_LOCATIONS)
		target |= RXRenderableUnit::STARTING_LOCATION;
	if(renderingMode & RENDER_DOODADS)
		target |= RXRenderableUnit::DOODADS;
	if(renderingMode & RENDER_BUILDINGS)
		target |= RXRenderableUnit::BUILDING_UNIT;

	for(int i=0;i<ct.getUnitCount();i++)
	{
		if(ct.getUnit(i)->getType() & target)
			ct.getUnit(i)->render(&renderer);
	}
}










