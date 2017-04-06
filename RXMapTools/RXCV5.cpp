#include "stdafx.h"
#include <rxmaptools/file/RXCV5.h>
#include <rxfile/RXFileManager.h>

RXCV5::RXCV5(std::istream *input)
{
	read(input);
}

RXCV5::~RXCV5(void)
{
}



void RXCV5::read(std::istream *input)
{
	unsigned int size = RXFileManager::getSize(input);

	index.reserve(size / 3);

	unsigned int read = 0;

	while(read < size)
	{

		char curBuf[32];

		// On skippe les 20 premiers octets de chaque entrée car on ne sait pas à quoi ils servent :] En revanche, les 32 suivants nous intéressent.
		input->seekg(20, std::ios::cur);
		input->read((char *)curBuf, 32);

		for(int i = 0; i < 32; i = i + 2)
		{
			int low = curBuf[i] & 255;		//haha lolzor. Je laisse, mais bon
			int hi  = curBuf[i + 1] & 255;

			index.push_back((hi<<8) | low);
		}

		read+= 20 + 32;
	}
}




