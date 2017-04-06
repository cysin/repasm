#ifndef __RX__HQMAPRENDERER
#define __RX__HQMAPRENDERER

#include <rxmaptools/RXMapConverter.h>

class RXFileManager;
class RXUnitLoader;

class RXHQMapConverter : public RXMapConverter
{
public:
	RXHQMapConverter(RXFileManager *res);
	~RXHQMapConverter(void);


private:

	std::auto_ptr<RXUnitLoader> unitLoader;
	virtual bool canRenderDoodads()				{ return true;}
	virtual bool canRenderRessources()			{ return true;}
	virtual bool canRenderUnits()				{ return true;}
	virtual bool canRenderStartingLocation()	{ return true;}

	virtual RXRenderableUnit * getDoodads(int x, int y, unsigned short doodadsType)						;
	virtual RXRenderableUnit * getUnit(int x, int y, RXMapEnum::UnitType type, unsigned short owner)	;
	virtual RXRenderableUnit * getMinerals(int x, int y, RXMapEnum::UnitType type, unsigned int amount)	;
	virtual RXRenderableUnit * getVespeneGeyser(int x, int y)											;
	virtual RXRenderableUnit * getStartingLocation(int x, int y, RXMapEnum::UnitType type, int colorID)	;
};

#endif
