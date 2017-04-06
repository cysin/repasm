#include "stdafx.h"
#include "BWTools/RXColor.h"

RXColor::RXColor()
{
}

RXColor::~RXColor(void)
{
}


void RXColor::serialize(std::ostream &str) {
	str.write((const char*)&colorID, sizeof(colorID));
}
