/*
* Changelog
* 
* 07/09/2007 - Removed the checkDimension() method because it did not have a real purpose (non-standard dimensions are OK in SC)
*/

#include "stdafx.h"
#include <sstream>
#include <rxmapbase/RXMap.h>
#include <rxmapbase/sections/RXMapDIMSection.h>
#include <rxmapbase/sections/RXMapSTRSection.h>
#include <rxmapbase/sections/RXMapSPRPSection.h>
#include <rxmapbase/sections/RXMapVERSection.h>

#include <fstream>

RXMapInvalidMapException::RXMapInvalidMapException(const std::string &f) : std::runtime_error(f.c_str())
{
}


RXMap::RXMap(void)
{
}

RXMap::~RXMap(void)
{
}



void RXMap::addSection(const std::string &name, RXMapSection *section)
{

	if (name == "MTXM")
		mergeMTXM(section);
	else if (name == "DIM " || name == "SPRP" || name == "STR ")
	{
		if (sectionHash[name] != NULL)
			delete sectionHash[name];

		sectionHash[name] = section;
	}
	else
	{
		if (sectionHash[name] != NULL)
			delete section; //first in, first used
		else
			sectionHash[name] = section;
	}

	list.push_back(section);
}




void RXMap::mergeMTXM(RXMapSection *section)
{
	RXMapSection * sec = getSection<RXMapSection>("MTXM");
	if (sec)
	{
		if (section->getSectionSize() < sec->getSectionSize())
		{
			memcpy(sec->getRawData(), section->getRawData(), section->getSectionSize());
			delete section;
		}
		else
		{
			memcpy(section->getRawData(), sec->getRawData(), sec->getSectionSize());
			delete sec;
			sectionHash["MTXM"] = section;
		}

		return;
	}
	else
		sectionHash["MTXM"] = section;
}






const RXMapSection * RXMap::getRawSection(const std::string &name) const
{
	return getSection<RXMapSection>(name);
}


const RXMapERASection * RXMap::getERA() const
{
	return getSection<RXMapERASection>("ERA ");
}

const RXMapDIMSection * RXMap::getDIM() const
{
	return getSection<RXMapDIMSection>("DIM ");
}

const RXMapUNITSection * RXMap::getUNIT() const
{
	return getSection<RXMapUNITSection>("UNIT");
}

const RXMapTHG2Section *RXMap:: getTHG2() const
{
	return getSection<RXMapTHG2Section>("THG2");}

const RXMapMTXMSection * RXMap::getMTXM() const
{
	return getSection<RXMapMTXMSection>("MTXM");
}

const RXMapVERSection * RXMap::getVER() const
{
	return getSection<RXMapVERSection>("VER ");
}




//Some shortcuts functions

int RXMap::getWidth() const
{
	const RXMapDIMSection *sec = getDIM();

	if(sec)
		return sec->getWidth();
	return -1;
}

int RXMap::getHeight()const
{
	const RXMapDIMSection *sec = getDIM();

	if(sec)
		return sec->getHeight();
	return -1;
}



std::string RXMap::getMapName() const
{
    const RXMapSPRPSection * sprp = getSection<RXMapSPRPSection>("SPRP");
    if (!sprp)
        return "Unnamed map";

    int index = sprp->getMapNameIndex();

    const RXMapSTRSection * str = getSection<RXMapSTRSection>("STR ");

    if (str)
    {
        const char * str2 = str->getString(index);
        return str2;

    }
    return "Unnamed map";
}


std::string RXMap::getMapDescription() const
{
    const RXMapSPRPSection * sprp = getSection<RXMapSPRPSection>("SPRP");
    if (!sprp)
        return "No description";

    int index = sprp->getMapDescriptionIndex();

    const RXMapSTRSection * str = getSection<RXMapSTRSection>("STR ");

    if (str)
        return str->getString(index);
    return "No description";
}




void RXMap::checkMap() const
{
	checkSectionExistance("DIM ");
	checkSectionExistance("ERA ");
	checkSectionExistance("UNIT");
	checkSectionExistance("MTXM");


	const RXMapDIMSection *sec = getDIM();
	const RXMapSection *mtxm = getSection<RXMapSection>("MTXM");

	int size = sec->getWidth() * sec->getHeight() * 2;

	if (mtxm->getSectionSize() != size)
		throw RXMapInvalidMapException("Invalid MTXM size");
}




void RXMap::checkSectionExistance(const std::string &name) const
{
	if (getSection<RXMapSection>(name)==NULL)
	{
		std::ostringstream str;
		str << "Section '"<<name<<"' not present";
		throw RXMapInvalidMapException(str.str());
	}
}


