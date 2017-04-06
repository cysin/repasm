#ifndef RX_MAPCONVERTER__H
#define RX_MAPCONVERTER__H

#include <rxmapbase/RXMapEnum.h>

//Converti le couple RXMap / RXReplay en RXRenderableMap


class RXReplay;
class RXRenderableMap;
class RXMap;
class RXRenderableUnit;


class RXMapConverter
{
public:
	virtual ~RXMapConverter(void);

	RXRenderableMap * convert(const RXMap *map, const RXReplay *replay=NULL);

private:

	void addPlayerStartingLocation(const RXMap *map, const RXReplay *replay, RXRenderableMap *dest);
	void addResources(const RXMap *map, RXRenderableMap *dest);
	void addDoodads(const RXMap *map, RXRenderableMap *dest);
	void addMapUnit(const RXMap *map, RXRenderableMap *dest);
	void addUnitsForUMSReplays(const RXMap *map, const RXReplay *replay, RXRenderableMap *dest);



	//asks derived class about capabilities
	virtual bool canRenderDoodads()				{ return false;}
	virtual bool canRenderRessources()			{ return false;}
	virtual bool canRenderUnits()				{ return false;}
	virtual bool canRenderStartingLocation()	{ return false;}

	virtual RXRenderableUnit * getDoodads(int x, int y, unsigned short doodadsType)						{return NULL;};
	virtual RXRenderableUnit * getUnit(int x, int y, RXMapEnum::UnitType type, unsigned short owner)	{return NULL;};
	virtual RXRenderableUnit * getMinerals(int x, int y, RXMapEnum::UnitType type, unsigned int amount)	{return NULL;};
	virtual RXRenderableUnit * getVespeneGeyser(int x, int y)											{return NULL;};
	virtual RXRenderableUnit * getStartingLocation(int x, int y, RXMapEnum::UnitType type, int colorID)	{return NULL;};





protected:
	RXMapConverter();




	inline bool isResource(RXMapEnum::UnitType type) const;
	inline bool isMineral(RXMapEnum::UnitType type)  const;
};

#endif

