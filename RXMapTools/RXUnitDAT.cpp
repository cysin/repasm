#include "stdafx.h"
#include <rxmaptools/file/RXUnitDAT.h>



RXUnitDAT::RXUnitDAT(std::istream *str)
{
	graphics		= NULL;
	animationLevel	= NULL;
	subunit1		= NULL;

	read(str);
}

RXUnitDAT::~RXUnitDAT(void)
{
	if (graphics)
		delete[] graphics;

	if (subunit1)
		delete []subunit1;


	if (animationLevel)
		delete []animationLevel;
}

	//thanks to .az file in Arsenal Zero







void RXUnitDAT::read(std::istream *input)
{
	readArray(input, graphics,		228);
	readArray(input, subunit1,		228);


	//Seek to animation level. 
	input->seekg(0xFE4,std::ios::beg);
	readArray(input, animationLevel,		228);

	//As of now, we are not interested in the rest of the file
}






template <typename T>
void RXUnitDAT::readArray(std::istream *input,T *&dest, int num)
{
	dest = new T[num];
	input->read((char *)dest, num * sizeof(T));
}
