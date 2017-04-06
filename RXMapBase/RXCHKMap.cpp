#include "stdafx.h"
#include <rxmapbase/RXCHKMap.h>

#include <rxmapbase/sections/RXMapDIMSection.h>
#include <rxmapbase/sections/RXMapERASection.h>
#include <rxmapbase/sections/RXMapUNITSection.h>
#include <rxmapbase/sections/RXMapTHG2Section.h>
#include <rxmapbase/sections/RXMapMTXMSection.h>
#include <rxmapbase/sections/RXMapSTRSection.h>
#include <rxmapbase/sections/RXMapVERSection.h>


RXCHKMap::RXCHKMap(std::istream *input)
{
	read(input);
	checkMap();
}

RXCHKMap::~RXCHKMap(void)
{
}






void RXCHKMap::readAndAddRXMapSection(std::istream *input, const char *sectionName, int size)
{
	RXMapSection * sec = new RXMapSection(sectionName, input, size);
	addSection(sectionName, sec);
}




template<typename T>
void RXCHKMap::readAndAddSection(std::istream *input, const char *sectionName, int size)
{
	T * sec = new T(input, size);
	addSection(sectionName, sec);
}



void RXCHKMap::read(std::istream *input)
{
	char sectionName[5];
	sectionName[4] = 0;

	while(input->eof() == false && input->fail()==false)
	{
		int pos = input->tellg();

		unsigned int sectionSize;

		input->read(sectionName,4);
		input->read((char *)&sectionSize,4);

		if (input->eof())
			break;

        if (sectionSize < 256 *1024)
        {
            if (strcmp(sectionName,"DIM ") == 0)
                readAndAddSection<RXMapDIMSection>(input, sectionName, sectionSize);
            else if (strcmp(sectionName,"ERA ") == 0)
                readAndAddSection<RXMapERASection>(input, sectionName, sectionSize);
            else if (strcmp(sectionName,"UNIT") == 0)
                readAndAddSection<RXMapUNITSection>(input, sectionName, sectionSize);
            else if (strcmp(sectionName,"THG2") == 0)
                readAndAddSection<RXMapTHG2Section>(input, sectionName, sectionSize);
            else if (strcmp(sectionName,"MTXM") == 0)
                readAndAddSection<RXMapMTXMSection>(input, sectionName, sectionSize);
            else if (strcmp(sectionName,"STR ") == 0)
                readAndAddSection<RXMapSTRSection>(input, sectionName, sectionSize);
			else if (strcmp(sectionName,"VER ") == 0)
				readAndAddSection<RXMapVERSection>(input, sectionName, sectionSize);
            else if (strcmp(sectionName,"uBeR") == 0)
                input->seekg(sectionSize,std::ios::cur);
            else
                readAndAddRXMapSection(input, sectionName, sectionSize);
        }
        else
            input->seekg(sectionSize,std::ios::cur);


	}
}












