#include "stdafx.h"
#include <rxmaptools/RXUnitLoader.h>
#include <rxmaptools/RXHQMapConverter.h>

RXHQMapConverter::RXHQMapConverter(RXFileManager *res) :
unitLoader(new RXUnitLoader(res))
{
}

RXHQMapConverter::~RXHQMapConverter(void)
{
}






RXRenderableUnit * RXHQMapConverter::getDoodads(int x, int y, unsigned short doodadsType)
{
	return unitLoader->getDoodads(x, y, doodadsType);
}



RXRenderableUnit * RXHQMapConverter::getUnit(int x, int y, RXMapEnum::UnitType type, unsigned short owner)	
{
	return unitLoader->getUnit(x, y, type, owner);
}

RXRenderableUnit * RXHQMapConverter::getMinerals(int x, int y, RXMapEnum::UnitType type, unsigned int amount)
{
	return unitLoader->getMinerals(x, y, type, amount);
}

RXRenderableUnit * RXHQMapConverter::getVespeneGeyser(int x, int y)
{
	return unitLoader->getUnit(x, y, RXMapEnum::UnitTypeValue::VESPENE_GEYSER, 0);
}

RXRenderableUnit * RXHQMapConverter::getStartingLocation(int x, int y, RXMapEnum::UnitType type, int colorID)
{
	return unitLoader->getStartingLocation(x, y, type, colorID);
}









