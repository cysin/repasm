#include "stdafx.h"
#include <rxmaptools/file/RXLO.h>

RXLO::RXLO(std::istream *str)
{


	str->read((char *)&frameCount,4);
	str->read((char *)&overlayPerFrame,4);

	str->seekg(4 * frameCount,std::ios::cur);

	data = new RXOverlayOffset[frameCount * overlayPerFrame];

	str->read((char *)data,frameCount * overlayPerFrame * sizeof(RXOverlayOffset));
}







RXLO::~RXLO(void)
{
	if (data)
		delete []data;
}



const RXLO::RXOverlayOffset * RXLO::getOffset(int frame, int overlay) const
{
	return data + frame * overlayPerFrame + overlay;
}



