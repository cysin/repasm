#include "stdafx.h"
#include <rxfile/RXFileManager.h>

#include <rxmaptools/RXLOLoader.h>
#include <rxmaptools/file/RXLO.h>

RXLOLoader::RXLOLoader(RXFileManager *fm) :
tbl(fm->getAutoFile("arr\\images.tbl").get())
{
	this->fm = fm;
}

RXLOLoader::~RXLOLoader(void)
{
}



const RXLO * RXLOLoader::loFromTBL(int tblID)
{
	tblID--;
	RXTLoHash::iterator it = loCache.find(tblID);

	if (it != loCache.end())
		return it->second;

	std::string name = "unit\\";
	name.append(tbl[tblID]);


	RXLO *res = new RXLO(fm->getAutoFile(name).get());

	loCache[tblID] = res;
	return res;
}
