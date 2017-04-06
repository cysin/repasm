#ifndef RX_MAPSECTIONVER__H
#define RX_MAPSECTIONVER__H

#include "RXMapSection.h"
#include <rxmapbase/RXMapEnum.h>

class RXMapVERSection : public RXMapSection
{
public:
	RXMapVERSection	(std::istream *str, int size);
	RXMapVERSection	(const unsigned char *data, int size);
	
	~RXMapVERSection();

	//Retrieves infos
	RXMapEnum::Version getVersion() const { return version;}
private:
	RXMapEnum::Version version;
};

#endif
