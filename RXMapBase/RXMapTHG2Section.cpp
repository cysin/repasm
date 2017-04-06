#include "stdafx.h"
#include <rxmapbase/sections/RXMapTHG2Section.h>

RXMapTHG2Section::RXMapTHG2Section(std::istream *str, int size) : RXMapSection("THG2",str, size) 
{
	createDoodads();
}

RXMapTHG2Section::RXMapTHG2Section(const unsigned char *data, int size) : RXMapSection("THG2",data, size) 
{
	createDoodads();
}




void RXMapTHG2Section::createDoodads()
{
	int numDoodads = getSectionSize() / DoodadsSize;

	for (int i=0;i<numDoodads;i++)
	{
		RXDoodads dst;
		getDoodad(dst, i);
		list.push_back(dst);
	}
}


void RXMapTHG2Section::getDoodad(RXMapTHG2Section::RXDoodads &dst, int ddNumber)
{

	int startOffset = ddNumber * DoodadsSize; 

	getValUpdateOffset<unsigned short,unsigned short>(dst.doodadUnit, startOffset);
	getValUpdateOffset<unsigned short,unsigned short>(dst.x, startOffset);
	getValUpdateOffset<unsigned short,unsigned short>(dst.y, startOffset);
	getValUpdateOffset<unsigned char,unsigned char>(dst.owner, startOffset);

	//what comes next doesn't interest us
}

