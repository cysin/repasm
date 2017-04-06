#include "stdafx.h"
#include <rxmaptools/file/RXTBL.h>
#include <rxfile/RXFileManager.h>

RXTBL::RXTBL(std::istream *ifstr)
{
	pData = NULL;
	read(ifstr);
}

RXTBL::~RXTBL(void)
{
	if (pData)
		delete []pData;
}





void RXTBL::read(std::istream *ifstr)
{

	int fileSize = RXFileManager::getSize(ifstr);

	unsigned short count;
	ifstr->read((char *)&count, 2);


	//read index
	unsigned short *index = new unsigned short[count];
	ifstr->read((char *)index,count * 2);

	//estimate file size
	int pos = count * 2+2;
	

	ifstr->seekg(pos, std::ios::beg);

	//read the whole string table
	pData = new char[(long)(fileSize - pos)+1];
	ifstr->read(pData,fileSize - pos);

	//and lastly, create index table
	str.reserve(count);

	//Gcc 2.95 fix
	for (int i=0;i<count;i++)
		str.push_back(pData + index[i] - pos); //

	delete []index;
}













