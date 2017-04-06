#include "stdafx.h"
#include <rxmaptools/RXBitmapCache.h>

#include <rxmaptools/RXGRPLoader.h>
#include <rxmaptools/file/RXGRP.h>
#include <rxmaptools/RXBitmap.h>
#include <rxmaptools/RXShadowPalette.h>
#include <rxmaptools/RXTeamPalette.h>
#include <rxmaptools/file/RXImageDAT.h>

#include <rxfile/RXFileManager.h>




RXBitmapCache::RXBitmapCache(RXFileManager *fm) : ressource(fm)
{
}

RXBitmapCache::~RXBitmapCache(void)
{
}











const RXBitmap * RXBitmapCache::getUnitBitmap(int imageID, int frame, int owner, float reductionFactor)
{
	using namespace RXMapEnum;

	if (image.get() == NULL)
		image.reset(new RXImageDAT(ressource->getAutoFile("arr\\images.dat").get()));

	Palette palette = image->getPalette(imageID);

	if (palette == PaletteValue::PALETTE_SHADOW || owner == -1)
		return getNeutralImage(imageID, frame, palette, reductionFactor);
	else
		return getNonNeutralImage(imageID, frame, owner, reductionFactor);

}







const RXBitmap *RXBitmapCache::getNeutralImage(int imageID, int frame, RXMapEnum::Palette palette, float reductionFactor)
{
	RXTNeutralImageKey key;


	key.value.frameNumber 	= frame;
	key.value.imageID		= imageID;
	key.ratio				= reductionFactor;

	const RXPalette *usePalette = this->activePalette;
	RXShadowPalette shadow;

	if (palette == RXMapEnum::PaletteValue::PALETTE_SHADOW)
		usePalette = &shadow;


	return getOrCreateImage(neutralHash, key,reductionFactor,usePalette);
}





const RXBitmap *RXBitmapCache::getNonNeutralImage(int imageID, int frame, int owner,  float reductionFactor)
{
	RXTNonNeutralImageKey key;


	key.value.frameNumber 	= frame;
	key.value.imageID		= imageID;
	key.value.teamID		= owner;
	key.ratio				= reductionFactor;

	RXTeamPalette palette(*activePalette,owner);

	return getOrCreateImage(nonNeutralHash, key,reductionFactor,&palette);
}




template<typename Key>
const RXBitmap *RXBitmapCache::getOrCreateImage(RXTPtrHash<Key, RXBitmap> &cache, Key key, float reductionFactor, const RXPalette * p)
{
	typename RXTPtrHash<Key, RXBitmap>::iterator it  = cache.find(key);

	//check if we have already processed this GRP
	if (it != cache.end())
		return it->second;


	//We haven't...
	//Check if we have it at resolution 1:1
	key.ratio = 1.0f;

	it  = cache.find(key);

	RXBitmap * originalBitmap;

	if (it != cache.end())
		originalBitmap =  it->second;
	else
	{
		//oops, we really have to load it

		if (grpLoader.get() == NULL)
			grpLoader.reset(new RXGRPLoader(ressource));

		const RXGRP  * grp = grpLoader->getImage(key.value.imageID);

		//Convert doodads to bitmap if possible
		if(grp->getFrameBitmap(key.value.frameNumber) != NULL)
		{
			originalBitmap = new RXBitmap(grp->getFrameBitmap(key.value.frameNumber),grp->getWidth(), grp->getHeight(),*p);

			//store it
			cache[key] = originalBitmap;
		}
		else
			originalBitmap = NULL;
	}

	if(originalBitmap != NULL)
	{
		if (reductionFactor <= 1.0)
			return originalBitmap;

		RXBitmap *resized = originalBitmap->minify(reductionFactor,getResizeQuality(reductionFactor));

		key.ratio	 = reductionFactor;
		cache[key] = resized;

		return resized;
	}

	return NULL;
}





int RXBitmapCache::getResizeQuality(float reductionFactor)
{
	if (reductionFactor > 4)
		return 2;
	if (reductionFactor > 2)
		return 4;
	if (reductionFactor > 1.5)
		return 8;
	return 16;
}























