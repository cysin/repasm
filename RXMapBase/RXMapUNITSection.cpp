#include "stdafx.h"
#include <rxmapbase/sections/RXMapUNITSection.h>

RXMapUNITSection::RXMapUNITSection(std::istream *str, int size) : RXMapSection("UNIT",str, size) 
{
	createUnit();
}

RXMapUNITSection::RXMapUNITSection(const unsigned char *data, int size) : RXMapSection("UNIT",data, size) 
{
	createUnit();
}


void RXMapUNITSection::createUnit()
{
	int numUnit = getSectionSize() / UnitSize;

	for (int i=0;i<numUnit;i++)
	{
		RXUnit dst;
		getUnit(dst, i);
		list.push_back(dst);
	}
}


void RXMapUNITSection::getUnit(RXMapUNITSection::RXUnit &dst, int unitNumber)
{

	int startOffset = unitNumber * UnitSize + 4; //skip the first int

	getValUpdateOffset<unsigned short>(dst.x, startOffset);
	getValUpdateOffset<unsigned short>(dst.y, startOffset);
	getValUpdateOffset<unsigned short>(dst.unitType, startOffset);

	//Skip some value
	startOffset += 2; //Unknown/unused
	startOffset += 2; //Flag of which special properties can be applied to unit, and are valid
	startOffset += 2; //Flag for which elements of the unit data are valid

	getValUpdateOffset<unsigned char>(dst.owner, startOffset);
	getValUpdateOffset<unsigned char>(dst.hitPoint, startOffset);
	getValUpdateOffset<unsigned char>(dst.shieldPoint, startOffset);
	getValUpdateOffset<unsigned char>(dst.energyPoint, startOffset);
	
	getValUpdateOffset<unsigned int>(dst.ressourceAmount, startOffset);

	//what comes next doesn't interest us
}


/*
struct RXUnit
{
	unsigned short x;
	unsigned short y;

	unsigned short unitType;

	unsigned char hitPoint;
	unsigned char shieldPoint;
	unsigned char energyPoint;

	unsigned int ressourceAmount;
};
*/

