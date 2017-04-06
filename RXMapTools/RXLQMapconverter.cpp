#include "stdafx.h"
#include  <rxmaptools/RXLQMapConverter.h>
#include "RXLQUnitLayer.h"
#include <rxmaptools/RXRenderableUnit.h>
#include <rxmaptools/RXTeamPalette.h>
/*
SL :  128 x 96
CC : 128x160
Geyser : 128x60
Hatch : 192x160
Mins : 64x96
Nexus : 192x224
*/


RXLQMapConverter::RXLQMapConverter(void)
{
}

RXLQMapConverter::~RXLQMapConverter(void)
{
}



RXRenderableUnit * RXLQMapConverter::getMinerals(int x, int y, RXMapEnum::UnitType type, unsigned int amount)
{
	return new RXRenderableUnit(x,y,4,RXRenderableUnit::RESOURCE,new RXLQUnitLayer(50,50, 0x19B5BF), 0);
}


RXRenderableUnit * RXLQMapConverter::getVespeneGeyser(int x, int y)
{
	return new RXRenderableUnit(x,y,4,RXRenderableUnit::RESOURCE,new RXLQUnitLayer(120,60, 0xFF00), 0);
}


RXRenderableUnit * RXLQMapConverter::getStartingLocation(int x, int y, RXMapEnum::UnitType type, int colorID)
{

	unsigned int color = RXTeamPalette::getTeamColor(colorID)[0];
	return new RXRenderableUnit(x,y,4,RXRenderableUnit::STARTING_LOCATION,new RXLQUnitLayer(110,110, color), 0);
}












