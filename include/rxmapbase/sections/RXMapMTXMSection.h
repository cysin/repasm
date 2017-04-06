#ifndef RX_MAPSECTIONMTXM__H
#define RX_MAPSECTIONMTXM__H


#include "RXMapSection.h"

class RXMapMTXMSection :
	public RXMapSection
{
public:
	RXMapMTXMSection(std::istream *str, int size);
	RXMapMTXMSection(const unsigned char *data, int size);

	~RXMapMTXMSection(void);

	const unsigned short * getTile() const;
};


#endif
