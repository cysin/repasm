#include "stdafx.h"
#include <rxmapbase/sections/RXMapSPRPSection.h>

RXMapSPRPSection::RXMapSPRPSection(std::istream *str, int size) : RXMapSection("SPRP", str, size)
{
}


RXMapSPRPSection::RXMapSPRPSection(const unsigned char *data, int size) : RXMapSection("SPRP", data, size)
{
}

RXMapSPRPSection::~RXMapSPRPSection(void)
{
}



int RXMapSPRPSection::getMapNameIndex() const
{
    const unsigned short * data = (const unsigned short * ) getRawData();
    return data[0];
}


int RXMapSPRPSection::getMapDescriptionIndex() const
{
    const unsigned short * data = (const unsigned short * ) getRawData();
    return data[1];
}

