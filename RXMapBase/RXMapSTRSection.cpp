#include "stdafx.h"
#include <rxmapbase/sections/RXMapSTRSection.h>

RXMapSTRSection::RXMapSTRSection(std::istream *str, int size) : RXMapSection("STR ", str, size)
{
}


RXMapSTRSection::RXMapSTRSection(const unsigned char *data, int size) : RXMapSection("STR ", data, size)
{
}

RXMapSTRSection::~RXMapSTRSection(void)
{
}





const char * RXMapSTRSection::getString(int index) const
{
	// Reading strings in STR section is like reading TBL :
	// the index (must double it if working in char ; using shorts should be OK) is the offset to a short pointing to the offset the string starts

    const unsigned short * data = (const unsigned short *)getRawData();

    if (index < 0 || index > data[0])
        return NULL;

	unsigned short strStart = data[index];

    return (const char *)data + strStart;
}
