#include "stdafx.h"
#include <rxmapbase/RXRepMap.h>
#include <BWlib/BWrepAPI.h>
#include <BWlib/BWrepMap.h>

#include <rxmapbase/sections/RXMapDIMSection.h>
#include <rxmapbase/sections/RXMapERASection.h>
#include <rxmapbase/sections/RXMapUNITSection.h>
#include <rxmapbase/sections/RXMapTHG2Section.h>
#include <rxmapbase/sections/RXMapMTXMSection.h>
#include <rxmapbase/sections/RXMapSTRSection.h>
#include <rxmapbase/sections/RXMapSPRPSection.h>
#include <rxmapbase/sections/RXMapVERSection.h>




void RXRepMap::addRXMapSection(const BWrepMapSection *section, const char *name)
{
	RXMapSection *sec = new RXMapSection(name, section->GetData(), section->GetSize());
	addSection(name, sec);
}




template <typename T>
void RXRepMap::addRepSection(const BWrepMapSection *section, const char *name)
{
	T *sec = new T(section->GetData(), section->GetSize());
	addSection(name, sec);
}



RXRepMap::RXRepMap(const BWrepFile &file)
{

	for (int i=0;i<file.m_oMap.GetNumSection();i++)
	{
		

		const BWrepMapSection * section = file.m_oMap.GetSectionNumber(i);

		const char *title		 = section->GetTitle();
		unsigned int sectionSize = section->GetSize();

		if (strcmp(title,"DIM ") == 0)
			addRepSection<RXMapDIMSection>(section, title);
		else if (strcmp(title,"ERA ") == 0)
			addRepSection<RXMapERASection>(section, title);
		else if (strcmp(title,"UNIT") == 0)
			addRepSection<RXMapUNITSection>(section, title);
		else if (strcmp(title,"THG2") == 0)
			addRepSection<RXMapTHG2Section>(section, title);
		else if (strcmp(title,"MTXM") == 0)
			addRepSection<RXMapMTXMSection>(section, title);
        else if (strcmp(title,"STR ") == 0)
            addRepSection<RXMapSTRSection>(section, title);
        else if (strcmp(title,"SPRP") == 0)
            addRepSection<RXMapSPRPSection>(section, title);
		else if (strcmp(title,"VER ") == 0)
			addRepSection<RXMapVERSection>(section, title);
        else if (strcmp(title,"uBeR") == 0) {
            //Do nothing
        }
		else
			addRXMapSection(section, title);
	}

	checkMap();
}

RXRepMap::~RXRepMap(void)
{
}















