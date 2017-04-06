#include "stdafx.h"
#include <rxmaptools/file/RXVX4.h>
#include <rxfile/RXFileManager.h>

RXVX4::RXVX4(std::istream *input)
{
	read(input);
}

RXVX4::~RXVX4(void)
{
}




void RXVX4::read(std::istream *input)
{

	int size = (int) RXFileManager::getSize(input);

	subTile.reserve(size/2);

	int read=0;

	while(size-read >= 32)
	{
		SubTileIndex tile;

		char curBuf[32];
		input->read(curBuf,32);

		int cpt = 0;
		for(int i = 0; i < 4; i++)
		{
			for(int j = 0; j < 4; j++)
			{
				// On décale de 8 bits à droite pour pouvoir additionner ensuite
				int low = curBuf[cpt] & 255;
				cpt++;
				int hi = curBuf[cpt] & 255;
				cpt++;

				tile.index[j][i] = (hi<<8) | low;
			}
		}
		read+=32;
		subTile.push_back(tile);
	}
}
