#include "stdafx.h"
#include "rxmapbase/RXStartingLocation.h"
#include "rxmapbase/RXMap.h"
#include "rxmapbase/RXMapEnum.h"
#include "rxmapbase/sections/RXMapUNITSection.h"
#include "BWlib/BWrepAPI.h"


//If I include <math.h> the linkers complains about duplicated symbol (cosf and sqrtf in repasm.obj). Don't know why
//By doing so I get things to work, even if this is really a stupid hack. Any explanations welcome
#ifdef STUPID_PHP
extern "C" {
       float  sqrtf( float );
       float  cosf( float );
}
#else
#include <math.h>
#endif

const int RXStartingLocation::m_start4[]     = {3,12,9};    
const int RXStartingLocation::m_invstart4[]  = {3,6,9};
const int RXStartingLocation::m_start12[]    = {3,2,1,12,11,10,9};    
const int RXStartingLocation::m_invstart12[] = {3,4,5,6,7,8,9};



//Compute the cosines of the angle between (center-(x,y) and (1,0)
float RXStartingLocation::getAngleCos(int mapsizeX, int mapSizeY,float x, float y) {
    float tx = x - (mapsizeX/2);
    float ty = y - (mapSizeY/2);
    return tx/sqrtf(tx * tx + ty * ty);
}



void RXStartingLocation::initCosArray(float *cosVal, int numSegment) {

    float angleInc = 3.1415f / (float)(numSegment-2);
    float angleDec = angleInc /2.0f;

    for(int i=0;i<numSegment-2;i++) {
        cosVal[1+i]= cosf(angleDec);
        angleDec+=angleInc;
    }
    cosVal[0]            = 1.0f;
    cosVal[numSegment-1] = -1.0f;
}


void RXStartingLocation::getStartPos(const BWrepFile &file, const BWrepPlayer &player, int &mx, int &my,int &x, int &y) {

	mx  = file.m_oHeader.getMapWidth() *32;
	my  = file.m_oHeader.getMapHeight()*32;

	const BWrepMapSectionUNIT  *unitSection = (const BWrepMapSectionUNIT *) file.m_oMap.GetSection(SECTION_UNIT);

	if(NULL != unitSection)
	{
		for (int i=0;i<unitSection->GetUnitCount();i++) 
		{
			BWrepMapSectionUNIT::BWrepUnitDesc *desc =  unitSection->GetUnitDesc(i);
			if (desc->unitid == BWrepMapSectionUNIT::UNIT_STARTLOCATION) 
			{
				if (desc->playerid==player.getNumber()) 
				{
					x = desc->x;
					y = desc->y;
					return;
				}
			}
		}
	}
	x = y = 0;
}




int RXStartingLocation::getStartingPosition4(const BWrepFile &file, const BWrepPlayer &player, RX2dCoord &coord) {


	int x,y;
	int mapSizeX, mapSizeY;


	getStartPos(file, player, mapSizeX, mapSizeY, x, y);

	coord.x = (float)x / (float)mapSizeX;
	coord.y = (float)y / (float)mapSizeY;

	float angleCos = getAngleCos(mapSizeX, mapSizeY, (float)x, (float)y);
    const int *rpos =m_start4;
    if (y > mapSizeY/2)
        rpos = m_invstart4;       

    float cosval[4];
    initCosArray(cosval,4);

    for(int i=1;i<4;i++) {
        if (cosval[i-1]>=angleCos && angleCos > cosval[i]) 
            return rpos[i-1];
    }



    return -1;
}



int RXStartingLocation::getStartingPosition12(const BWrepFile &file, const BWrepPlayer &player,RX2dCoord &coord) {


	int x,y;
	int mapSizeX, mapSizeY;


	getStartPos(file, player, mapSizeX, mapSizeY, x, y);

	coord.x = (float)x / (float)mapSizeX;
	coord.y = (float)y / (float)mapSizeY;


    float angleCos = getAngleCos(mapSizeX, mapSizeY, (float)x, (float)y);

    const int *rpos =m_start12;
    if (y > mapSizeY/2)
        rpos = m_invstart12;             

    float cosval[8];
    initCosArray(cosval,8);

    for(int i=1;i<8;i++) {
        if (cosval[i-1]>=angleCos && angleCos > cosval[i]) 
            return rpos[i-1];
    }

    return -1;
}

std::vector<int> RXStartingLocation::listStartingPositions12(const BWrepFile &file)
{
	std::vector<int> result;

	int mx  = file.m_oHeader.getMapWidth() *32;
	int my  = file.m_oHeader.getMapHeight()*32;

	const BWrepMapSectionUNIT  *unitSection = (const BWrepMapSectionUNIT *) file.m_oMap.GetSection(SECTION_UNIT);

	if(NULL != unitSection)
	{
		for (int i=0;i<unitSection->GetUnitCount();i++) 
		{
			BWrepMapSectionUNIT::BWrepUnitDesc *desc =  unitSection->GetUnitDesc(i);
			if (desc->unitid == BWrepMapSectionUNIT::UNIT_STARTLOCATION)
			{
				int pos = computeStartPos12(desc->x, desc->y, mx, my);
				if(pos > 0)
					result.push_back(pos);
			}
		}
	}

	return result;
}

std::vector<int> RXStartingLocation::listStartingPositions12(const RXMap *map)
{
	std::vector<int> result;

	int mx  = map->getWidth() *32;
	int my  = map->getHeight()*32;

	const RXMapUNITSection  *unitSection = map->getUNIT();

	if(NULL != unitSection)
	{
		const RXMapUNITSection::RXTUnitList units = unitSection->getUnitList();
		for (unsigned int i = 0 ; i < units.size() ; i++) 
		{
			RXMapUNITSection::RXUnit curUnit = units[i];
			if (curUnit.unitType == RXMapEnum::UnitTypeValue::START_LOCATION)
			{
				int pos = computeStartPos12(curUnit.x, curUnit.y, mx, my);
				if(pos > 0)
					result.push_back(pos);
			}
		}
	}

	return result;
}

int RXStartingLocation::computeStartPos12(unsigned short x, unsigned short y, int mapsizeX, int mapsizeY)
{
	float angleCos = getAngleCos(mapsizeX, mapsizeY, (float)x, (float)y);

	const int *rpos =m_start12;
	if (y > mapsizeY/2)
		rpos = m_invstart12;

	float cosval[8];
	initCosArray(cosval,8);

	for(int i=1;i<8;i++) {
		if (cosval[i-1]>=angleCos && angleCos > cosval[i]) 
			return rpos[i-1];
	}

	return -1;
}
