#ifndef RX_MAPSECTIONDIM__H
#define RX_MAPSECTIONDIM__H

#include "RXMapSection.h"

class RXMapDIMSection : public RXMapSection
{
public:
	RXMapDIMSection	(std::istream *str, int size);
	RXMapDIMSection	(const unsigned char *data, int size);
	
	~RXMapDIMSection();

	//Retrieves infos
	int getWidth() const { return width;}
	int getHeight()const { return height;}
private:
	int width;
	int height;
};

#endif
