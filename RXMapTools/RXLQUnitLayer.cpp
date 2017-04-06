#include "stdafx.h"
#include "./RXLQUnitLayer.h"
#include <rxmaptools/RXUnitRenderer.h>

RXLQUnitLayer::RXLQUnitLayer(int width, int height, unsigned int color)
{
	this->width  = width;
	this->height = height;

	this->color = color;
}

RXLQUnitLayer::~RXLQUnitLayer(void)
{
}



void RXLQUnitLayer::render(RXUnitRenderer *to,int x,int y,int owner) const
{
	to->fillRect(x, y, width, height, color);
}
