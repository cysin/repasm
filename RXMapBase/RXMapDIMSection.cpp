#include "stdafx.h"
#include <rxmapbase/sections/RXMapDIMSection.h>

RXMapDIMSection::RXMapDIMSection(std::istream *str, int size)
	: RXMapSection("DIM ",str, size)
{
	setVal<unsigned short>(width, 0);
	setVal<unsigned short>(height, 2);
}


RXMapDIMSection::RXMapDIMSection(const unsigned char *data, int size)
	: RXMapSection	("DIM ",data, size)
{
	setVal<unsigned short>(width, 0);
	setVal<unsigned short>(height, 2);
}



RXMapDIMSection::~RXMapDIMSection(void)
{
}

