#include "stdafx.h"
#include <rxmapbase/sections/RXMapSection.h>

RXMapSection::RXMapSection(const std::string &nm,std::istream *str, int size) : name(nm)
{
	pRawData = new unsigned char[size];
	str->read((char *)pRawData, size);

	this->sectionSize = size;
}

RXMapSection::RXMapSection(const std::string &nm,const unsigned char *data, int size) : name(nm)
{
	pRawData = new unsigned char[size];
	memcpy(pRawData, data, size);

	this->sectionSize = size;
}



RXMapSection::~RXMapSection(void)
{
	if (pRawData)
		delete []pRawData;
}
