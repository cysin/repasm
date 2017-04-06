#include "stdafx.h"
#include <rxmapbase/sections/RXMapMTXMSection.h>

RXMapMTXMSection::RXMapMTXMSection(std::istream *str, int size) : RXMapSection("MTXM",str, size)
{
}


RXMapMTXMSection::RXMapMTXMSection(const unsigned char *data, int size) : RXMapSection("MTXM",data, size)
{
}


RXMapMTXMSection::~RXMapMTXMSection(void)
{
}


const unsigned short * RXMapMTXMSection::getTile() const
{
	return (const unsigned short *)getRawData();
}

