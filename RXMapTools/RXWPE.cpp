#include "stdafx.h"
#include <rxmaptools/file/RXWPE.h>

RXWPE::RXWPE(std::istream *input)
{
	read(input);
}

RXWPE::~RXWPE(void)
{
}


void RXWPE::read(std::istream *input)
{

	for (int i=0;i<256;i++)
	{
		unsigned char bgra[4];
		Color col;

		input->read((char *)bgra,4);

		col.component.r = bgra[0];
		col.component.g = bgra[1];
		col.component.b = bgra[2];

		if (i==0)
			col.component.a = 0;
		else
			col.component.a = 255;

		setColor(i, col);
	}

}
