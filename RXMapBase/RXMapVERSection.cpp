#include "stdafx.h"
#include <rxmapbase/sections/RXMapVERSection.h>

RXMapVERSection::RXMapVERSection(std::istream *str, int size) : RXMapSection("VER ",str, size) 
{
	unsigned short v = getVal<unsigned short, unsigned short>();
	version = (RXMapEnum::Version)v;
}

RXMapVERSection::RXMapVERSection(const unsigned char *data, int size) : RXMapSection("VER ",data, size) 
{
	unsigned short v = getVal<unsigned short, unsigned short>();
	version = (RXMapEnum::Version)v;
}

RXMapVERSection::~RXMapVERSection()
{
}
