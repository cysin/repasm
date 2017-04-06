#ifndef RX_MAPSECTIONERA__H
#define RX_MAPSECTIONERA__H


#include "RXMapSection.h"

#include <rxmapbase/RXMapEnum.h>


class RXMapERASection :	public RXMapSection
{
public:
	RXMapERASection(std::istream *str, int size);
	RXMapERASection(const unsigned char *data, int size);

	~RXMapERASection(void);


	//Retrieves infos
	RXMapEnum::TileSet getTileSet() const { return tileSet;}

	
private:
	RXMapEnum::TileSet tileSet;

	void setTileSet();
};

#endif
