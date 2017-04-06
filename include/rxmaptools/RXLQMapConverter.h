#ifndef __RX_LQLMAPCONVERTER
#define __RX_LQLMAPCONVERTER

#include <rxmaptools/RXMapConverter.h>

//Convert into a renderable map using lo-quality settings, optimised for very small map images
//Only renders resource and starting location. No doodads or units. Overlay are rendered as colored 
//squares. 


class RXLQMapConverter : public RXMapConverter
{
public:
	RXLQMapConverter(void);
	~RXLQMapConverter(void);


private:
	virtual bool canRenderRessources()			{ return true;}
	virtual bool canRenderStartingLocation()	{ return true;}

	virtual RXRenderableUnit * getMinerals(int x, int y, RXMapEnum::UnitType type, unsigned int amount);
	virtual RXRenderableUnit * getVespeneGeyser(int x, int y);
	virtual RXRenderableUnit * getStartingLocation(int x, int y, RXMapEnum::UnitType type, int colorID);



};


#endif
