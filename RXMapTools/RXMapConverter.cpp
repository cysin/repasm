#include "stdafx.h"
#include <rxmaptools/RXMapConverter.h>
#include <rxmaptools/RXUnitLoader.h>
#include <rxmaptools/RXRenderableMap.h>

#include <rxmapbase/sections/RXMapUNITSection.h>
#include <rxmapbase/sections/RXMapDIMSection.h>
#include <rxmapbase/sections/RXMapERASection.h>
#include <rxmapbase/sections/RXMapMTXMSection.h>
#include <rxmapbase/sections/RXMapTHG2Section.h>
#include <rxmapbase/RXMap.h>

#include <BWTools/RXReplay.h>
#include <BWTools/RXPlayer.h>
#include <BWTools/RXRace.h>
#include <BWTools/RXColor.h>

RXMapConverter::RXMapConverter()
{
}

RXMapConverter::~RXMapConverter(void)
{
}






RXRenderableMap * RXMapConverter::convert(const RXMap *map, const RXReplay *replay)
{

	int height = map->getHeight();
	int width = map->getWidth();

	const unsigned short *tile = (const unsigned short *)map->getMTXM()->getRawData();

	RXMapEnum::TileSet ts = map->getERA()->getTileSet();

	RXRenderableMap * renderMap = new RXRenderableMap(width, height, tile, ts);

	
	
	addResources(map, renderMap);
	addDoodads(map, renderMap);

	//No replay or UMS-played game
	if (replay == NULL)
		addMapUnit(map, renderMap);
	else if(replay->getGameType() == RXReplay::USE_MAP_SETTINGS)
		addUnitsForUMSReplays(map, replay, renderMap);
	else
		addPlayerStartingLocation(map, replay, renderMap);

	return renderMap;
}









void RXMapConverter::addPlayerStartingLocation(const RXMap *map, const RXReplay *replay,RXRenderableMap *dest)
{
	using namespace RXMapEnum::UnitTypeValue;

	
	const RXMapUNITSection *unit = map->getUNIT();
	const RXMapUNITSection::RXTUnitList & list = unit->getUnitList();

	const RXPlayerList & playersList = replay->getPlayers();

	for (size_t i=0;i<list.size();i++)
	{
		const RXMapUNITSection::RXUnit &u = list[i];

		if (u.unitType == START_LOCATION)
		{
			for (size_t j=0;j<playersList.size();j++)
			{
				const RXPlayer *pl = playersList[j];

				if (pl->getPlayerID() == u.owner && !pl->isObs())
				{
					RXRenderableUnit * renderable = NULL;
					switch(pl->getRace()->getRaceID())
					{
					case RXRace::RACE_PROTOSS:
						renderable = getStartingLocation(u.x, u.y, PROTOSS_NEXUS, pl->getColor()->getColorID());
						break;
					case RXRace::RACE_TERRAN:
						renderable = getStartingLocation(u.x, u.y, TERRAN_COMMAND_CENTER, pl->getColor()->getColorID());
						break;
					case RXRace::RACE_ZERG:
						renderable = getStartingLocation(u.x, u.y,ZERG_HATCHERY, pl->getColor()->getColorID());
						break;
					}

					if (renderable)
						dest->getUnit().addUnit(renderable);

					break;
				}
			}
		}
	}
}





void RXMapConverter::addUnitsForUMSReplays(const RXMap *map, const RXReplay *replay, RXRenderableMap *dest)
{
	using namespace RXMapEnum::UnitTypeValue;

	const RXMapUNITSection *unit = map->getUNIT();
	const RXMapUNITSection::RXTUnitList & list = unit->getUnitList();

	const RXPlayerList & playersList = replay->getPlayers();

	for (size_t i=0;i<list.size();i++)
	{
		const RXMapUNITSection::RXUnit &u = list[i];

		if (u.unitType == START_LOCATION)
		{
			for (size_t j=0;j<playersList.size();j++)
			{
				const RXPlayer *pl = playersList[j];

				if (pl->getPlayerID() == u.owner && !pl->isObs())
				{
					RXRenderableUnit * renderable = NULL;
					switch(pl->getRace()->getRaceID())
					{
					case RXRace::RACE_PROTOSS:
						renderable = getStartingLocation(u.x, u.y, PROTOSS_NEXUS, pl->getColor()->getColorID());
						break;
					case RXRace::RACE_TERRAN:
						renderable = getStartingLocation(u.x, u.y, TERRAN_COMMAND_CENTER, pl->getColor()->getColorID());
						break;
					case RXRace::RACE_ZERG:
						renderable = getStartingLocation(u.x, u.y,ZERG_HATCHERY, pl->getColor()->getColorID());
						break;
					}

					if (renderable)
						dest->getUnit().addUnit(renderable);

					break;
				}
			}
		}
		else if(isResource(u.unitType) == false)
		{
			RXRenderableUnit * renderable = getUnit(u.x, u.y, u.unitType, u.owner);//unitLoader->getUnit;

			if (renderable)
				dest->getUnit().addUnit(renderable);
		}
	}
}



void RXMapConverter::addDoodads(const RXMap *map, RXRenderableMap *dest)
{
	const RXMapTHG2Section *sec = map->getTHG2();
	if (!sec)
		return;

	const RXMapTHG2Section::RXTDoodadsList &list = sec->getDoodadsList();


	for(size_t i=0;i<list.size();i++)
	{
		const RXMapTHG2Section::RXDoodads & d = list[i];
		if (d.owner == 11)
		{
			RXRenderableUnit * unit = getUnit(d.x, d.y, (RXMapEnum::UnitType) d.doodadUnit, 1);//unitLoader->getUnit;
			if (unit)
				dest->getUnit().addUnit(unit);
		}
		else
		{
			RXRenderableUnit *unit = getDoodads(d.x, d.y, d.doodadUnit);
			if (unit)
				dest->getUnit().addUnit(unit);
		}
	}
}



void RXMapConverter::addMapUnit(const RXMap *map, RXRenderableMap *dest)
{
	using namespace RXMapEnum::UnitTypeValue;

	const RXMapUNITSection *unit = map->getUNIT();
	const RXMapUNITSection::RXTUnitList & list = unit->getUnitList();

	for (size_t i=0;i<list.size();i++)
	{
		const RXMapUNITSection::RXUnit &u = list[i];

		if (isResource(u.unitType) == false)
		{
			RXRenderableUnit * renderable = NULL;

			if (u.unitType == START_LOCATION)
				renderable = getStartingLocation(u.x, u.y,START_LOCATION,u.owner);
			else
				renderable = getUnit(u.x, u.y, u.unitType, u.owner);//unitLoader->getUnit;

			if (renderable)
				dest->getUnit().addUnit(renderable);
		}
	}
}


void RXMapConverter::addResources(const RXMap *map, RXRenderableMap *dest)
{
	const RXMapUNITSection *unit = map->getUNIT();
	const RXMapUNITSection::RXTUnitList & list = unit->getUnitList();

	for (size_t i=0;i<list.size();i++)
	{
		const RXMapUNITSection::RXUnit &u = list[i];

		if (isResource(u.unitType))
		{
			RXRenderableUnit * renderable = NULL;

			if (isMineral(u.unitType))
				renderable = getMinerals(u.x, u.y, u.unitType, u.ressourceAmount); //unitLoader->
			else
				renderable = getVespeneGeyser(u.x, u.y);

			if (renderable)
				dest->getUnit().addUnit(renderable);
		}
	}
}






inline bool RXMapConverter::isResource(RXMapEnum::UnitType type) const
{
	using namespace RXMapEnum::UnitTypeValue;

	if (isMineral(type))
		return true;

	if (type == VESPENE_GEYSER)
		return true;

	return false;
}

inline bool RXMapConverter::isMineral(RXMapEnum::UnitType type) const
{
	using namespace RXMapEnum::UnitTypeValue;

	switch(type)
	{
	case MINERAL_FIELD_TYPE_1: 
	case MINERAL_FIELD_TYPE_2: 
	case MINERAL_FIELD_TYPE_3:
		return true;
	default:
		return false;
	}
}



