#include "stdafx.h"
#include <rxmaptools/RXUnitLoader.h>

#include <rxfile/RXFileManager.h>

#include <rxmaptools/RXRenderableUnit.h>
#include <rxmaptools/file/RXIScript.h>
#include <rxmaptools/file/RXImageDAT.h>
#include <rxmaptools/file/RXUnitDAT.h>
#include <rxmaptools/RXLOLoader.h>

#include "RXUnitLayerImpl.h"


#include <rxmaptools/file/RXLO.h>


// Event when parsing a script



RXUnitLoader::RXUnitLoaderEvent::RXUnitLoaderEvent(RXUnitLoader *f, RXUnitLayerImpl *layer)
{
	this->fetch  = f;
	this->layer = layer;
}

void RXUnitLoader::RXUnitLoaderEvent::OnActiveUnderlay(unsigned short imageID, unsigned char xOffset, unsigned char yOffset)
{
	layer->addUnderlay(fetch->getLayer(imageID, yOffset));
};

void RXUnitLoader::RXUnitLoaderEvent::OnActiveOverlay(unsigned short imageID, unsigned char xOffset, unsigned char yOffset)
{
	layer->addOverlay(fetch->getLayer(imageID, yOffset));
};

void RXUnitLoader::RXUnitLoaderEvent::OnSetFrame(unsigned short imageNumber)
{
	layer->setActiveFrame(imageNumber);
};

void RXUnitLoader::RXUnitLoaderEvent::OnUseParentFrame()
{
	layer->setActiveFrame(255);
}
















RXUnitLoader::RXUnitLoader(RXFileManager *fm) :
script(new RXIScript(fm->getAutoFile("scripts\\iscript.bin").get())),
flingy(new RXTFlingyDat(fm->getAutoFile("arr\\flingy.dat").get(),227)),
sprite(new RXTSpriteDat(fm->getAutoFile("arr\\sprites.dat").get(),517)),
images(new RXImageDAT(fm->getAutoFile("arr\\images.dat").get())),
units(new  RXUnitDAT(fm->getAutoFile("arr\\units.dat").get())),
loCache(new RXLOLoader(fm))
{
}

RXUnitLoader::~RXUnitLoader(void)
{
}





RXRenderableUnit * RXUnitLoader::getDoodads(int x, int y, int doodadsID)
{
	RXUnitLayerImpl * mainLayer = getLayerFromSpriteID(doodadsID);
	return new RXRenderableUnit(x, y, 4, RXRenderableUnit::DOODADS, mainLayer,-1);
}




RXRenderableUnit * RXUnitLoader::getMinerals(int x, int y, RXMapEnum::UnitType unitID, int mineralAmount)
{
	RXUnitLayerImpl * mainLayer = getLayerFromUnitID(unitID);

	//tweak frame number to match the minerals amount
	if (mineralAmount>750)
		mineralAmount = 750;

	mainLayer->setActiveFrame(3 - mineralAmount / 250);

	return new RXRenderableUnit(x, y, 4, RXRenderableUnit::RESOURCE, mainLayer,-1);
}

RXRenderableUnit * RXUnitLoader::getStartingLocation(int x, int y, RXMapEnum::UnitType unitID, int colorID)
{
	RXUnitLayerImpl * mainLayer = getLayerFromUnitID(unitID);

	return new RXRenderableUnit(x, y, 4, RXRenderableUnit::STARTING_LOCATION, mainLayer, colorID);
}



RXRenderableUnit * RXUnitLoader::getUnit(int x, int y, RXMapEnum::UnitType unitID, int owner)
{
	int uid = (int)unitID;

	RXUnitLayerImpl * mainLayer = getLayerFromUnitID(uid);
	RXUnitLayerImpl * subLayer  = NULL;

	if (units->hasSubunit(uid))
	{
		subLayer = getLayerFromUnitID(units->getSubunit(uid));

		//charge le LO
		int imageID = getImageIndexFromImage(uid);
		int overlay = images->getOverlay4(imageID);

		const RXLO * lo = loCache->loFromTBL(overlay);

		const RXLO::RXOverlayOffset * offset = lo->getOffset(0,0);
		subLayer->setOffset(offset->x, offset->y);
	}

	fixUnit(mainLayer, uid);

	return new RXRenderableUnit(x, y, units->getAnimLevel(uid), unitToType(unitID), mainLayer,owner, subLayer);
}




RXUnitLayerImpl * RXUnitLoader::getLayerFromUnitID(int unitID)
{
	int imageIndex = getImageIndexFromImage(unitID);
	return getLayer(imageIndex, 0);
}


RXUnitLayerImpl * RXUnitLoader::getLayerFromSpriteID(int unitID)
{
	int imageIndex = (*sprite)[unitID];
	return getLayer(imageIndex, 0);
}



RXUnitLayerImpl * RXUnitLoader::getLayer(int imageID, int yOffset)
{
	
	
	RXUnitLayerImpl * res = new RXUnitLayerImpl(imageID, images->getPalette(imageID), yOffset);

	unsigned int scriptID = images->getScript(imageID);
	const unsigned char *sc= script->getScript(scriptID, RXIScript::INITIAL);

	if (sc)
	{
		RXUnitLoaderEvent event(this, res);
		RXScriptOpcodeReader reader;
		reader.read(sc, &event);
	}

	return res;
}





int RXUnitLoader::getImageIndexFromImage(int unit)
{
	int flingyLevel = units->getImage(unit);
	return (*sprite)[(*flingy)[flingyLevel]];
}




RXRenderableUnit::RXType RXUnitLoader::unitToType(RXMapEnum::UnitType type)
{
	using namespace RXMapEnum;



	switch(type)
	{
	case UnitTypeValue::START_LOCATION:
			return RXRenderableUnit::STARTING_LOCATION;

	case UnitTypeValue::MINERAL_FIELD_TYPE_1:
	case UnitTypeValue::MINERAL_FIELD_TYPE_2:
	case UnitTypeValue::MINERAL_FIELD_TYPE_3:
	case UnitTypeValue::VESPENE_GEYSER:
		return RXRenderableUnit::RESOURCE;

	default:
		return RXRenderableUnit::BUILDING_UNIT;
	}
}



void RXUnitLoader::fixUnit(RXUnitLayerImpl *impl, int uid)
{
	using namespace RXMapEnum::UnitTypeValue;

	//Some units doesn't load properly. Sometimes it comes from faulty script.bin decoding (even ICE seems to have some problems, or I 
	//didn't understand everything), so we hardfix this here. Of course this is not really nice, but it may do for a first version ....


	if (uid == TERRAN_REFINERY)
	{
		//set the correct frame. For some reasing iscript.bin decoding insist on putting frame 1
		//which show a not-finised refinery 

		impl->setActiveFrame(0); 
	}
	else if (uid == PROTOSS_CARRIER)
	{
		//Carrier won't load its shadow. That's dumb. Fix it the hard way
		RXUnitLayerImpl *shadow = getLayer(113,42); //113 is the shadow entry, 42 is the offset used by terran BC's shadow. Ugly hardcode, I know.
		impl->addUnderlay(shadow);
	}
}




























