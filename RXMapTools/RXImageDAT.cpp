#include "stdafx.h"
#include <rxmaptools/file/RXImageDAT.h>

RXImageDAT::RXImageDAT(std::istream *input)
{
	read(input);
}


RXImageDAT::~RXImageDAT(void)
{
	delete []tblEntry;
	delete []palette;
	delete []scriptID;
	delete []overlay4;
}





template <typename T>
void RXImageDAT::readArray(std::istream *input,T *&dest, int num)
{
	dest = new T[num];
	input->read((char *)dest, num * sizeof(T));
}


void RXImageDAT::read(std::istream *input)
{
	readArray(input, tblEntry,		999);

	input->seekg(1 * 999,std::ios::cur); //GFX Turns
	input->seekg(1 * 999,std::ios::cur); //Shadow Turns
	input->seekg(1 * 999,std::ios::cur); //Unknown 4
	input->seekg(1 * 999,std::ios::cur); //Floats

	readArray(input, palette,	999);

	input->seekg(1 * 999,std::ios::cur); //Palette special

	int pos = input->tellg();
	readArray(input, scriptID,	999);

	input->seekg(4 * 999,std::ios::cur); //shieldOverlay
	input->seekg(4 * 999,std::ios::cur); //overlay 2
	input->seekg(4 * 999,std::ios::cur); //overlay 3

	readArray(input, overlay4,	999);
}



RXMapEnum::Palette RXImageDAT::getPalette(int id) const
{
	using namespace RXMapEnum;

	switch(palette[id])
	{
	case 10: return PaletteValue::PALETTE_SHADOW;
	default: return PaletteValue::PALETTE_NORMAL;
	}
}











