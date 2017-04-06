#include "stdafx.h"
#include <rxmaptools/RXShadowPalette.h>

RXShadowPalette::RXShadowPalette(void)
{
	setColor(0,0);

	for (int i=1;i<256;i++)
		setColor(i, 0x7F000000);
}

RXShadowPalette::~RXShadowPalette(void)
{
}
