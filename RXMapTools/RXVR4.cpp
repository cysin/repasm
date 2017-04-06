#include "stdafx.h"
#include <rxmaptools/file/RXVR4.h>

#include <rxfile/RXFileManager.h>


RXVR4::RXVR4(std::istream *input)
{
	read(input);
}

RXVR4::~RXVR4(void)
{
}



void RXVR4::read(std::istream *input)
{
	long size = (long)RXFileManager::getSize(input);
	int numTile = size / (8*8);

	tile.reserve(numTile);

	for (int i=0;i<numTile;i++)
	{

		Tile t;
		input->read((char *)&t.val , 8*8);
		tile.push_back(t);
	}
}

