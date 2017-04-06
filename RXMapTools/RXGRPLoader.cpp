#include "stdafx.h"
#include <rxmaptools/RXGRPLoader.h>
#include <rxmaptools/file/RXTBL.h>
#include <rxmaptools/file/RXGRP.h>
#include <rxfile/RXFileManager.h>

  
RXGRPLoader::RXGRPLoader(RXFileManager *fm2) : 
		fm(fm2),

		//Make sure we have everything we needs to work
		flingy(new RXTFlingyDat(fm2->getAutoFile("arr\\flingy.dat").get(),227)),
		sprite(new RXTSpriteDat(fm2->getAutoFile("arr\\sprites.dat").get(),517)),
		images(new RXTImageDat(fm2->getAutoFile("arr\\images.dat").get(),999)),
		units(new RXTUnitsDat(fm2->getAutoFile("arr\\units.dat").get(),227)),
		tbl(new RXTBL(fm2->getAutoFile("arr\\images.tbl").get()))
{

}

RXGRPLoader::~RXGRPLoader(void)
{
}


const RXGRP * RXGRPLoader::getImage(int id)
{
	int index = (*images)[id];
	return getFromTBLIndex(index, true);
}

const RXGRP * RXGRPLoader::getDoodads(int spriteID)
{
	int img = (*sprite)[spriteID];
	int index = (*images)[img];
	return getFromTBLIndex(index, false);
}


const RXGRP * RXGRPLoader::getDoodadsShadow(int spriteID)
{
	int index = (*images)[(*sprite)[spriteID]];
	return getFromTBLIndex(index+1, false);
}




const RXGRP * RXGRPLoader::getUnits(int unitID)
{
	int index = (*images)[(*sprite)[(*flingy)[(*units)[unitID]]]];
	return getFromTBLIndex(index, true);
}

const RXGRP * RXGRPLoader::getUnitsShadow(int unitID)
{
	int flin = (*flingy)[(*units)[unitID]];
	int sptrIndex = (*sprite)[(*flingy)[(*units)[unitID]]];

	int index = (*images)[(*sprite)[(*flingy)[(*units)[unitID]]]] ;

	const char *match = "Shad.grp";
	const char *match2 = "Sha.grp";
	for (int i=0;i<10;i++)
	{
		const char * name = (*tbl)[(*images)[sptrIndex+i]-1];
		const char *name2 = name + strlen(name) - strlen(match);
		if (strcmp(name2, match)==0)
			return getFromTBLIndex(index+i+2, true);

		name2 = name + strlen(name) - strlen(match2);
		if (strcmp(name2, match2)==0)
			return getFromTBLIndex(index+i+2, true);
	}

	return getUnits(unitID);
}



RXGRP *RXGRPLoader::getFromTBLIndex(int tblid, bool allFrame)
{
	tblid--;
	RXTGRPHash::iterator it = grpHash.find(tblid);

	if (it != grpHash.end())
		return it->second;

	std::string str = std::string("unit\\")+(*tbl)[tblid];

	RXGRP *grp = new RXGRP(fm->getAutoFile(str).get(),allFrame);
	grpHash[tblid] = grp;

	return grp;
}



RXGRPLoader::RXTGRPHash::~RXTGRPHash()
{

	RXTGRPHash::iterator it = this->begin();

	while(it != this->end())
	{
		delete it->second;
		++it;
	}
}






