#include "stdafx.h"
#include "RXUnitLayerImpl.h"
#include <rxmaptools/RXUnitRenderer.h>


RXUnitLayerImpl::RXUnitLayerImpl(int imageID,  RXMapEnum::Palette palette, int yOffset)
{
	this->overlayX = this->overlayY = 0;
	this->imageID = 0;
	this->frameNumber = 0;
	this->yOffset = 0;


	this->imageID = imageID;
	this->yOffset = yOffset;
	this->palette = palette;
}

RXUnitLayerImpl::~RXUnitLayerImpl(void)
{
	for (size_t i=0;i<overlay.size();i++)
		delete overlay[i];

	for (size_t i=0;i<underlay.size();i++)
		delete underlay[i];
}




void RXUnitLayerImpl::setActiveFrame		 (int fc)
{
	this->frameNumber = fc;
}

void RXUnitLayerImpl::setFollowParentFrame()
{
	this->frameNumber = 0xFF;
}


void RXUnitLayerImpl::addOverlay (RXUnitLayerImpl *overlay)
{
//	overlay->parent = this;
	this->overlay.push_back(overlay);
}


void RXUnitLayerImpl::addUnderlay(RXUnitLayerImpl *underlay)
{
//	underlay->parent = this;
	this->underlay.push_back(underlay);
}



void RXUnitLayerImpl::setOffset(int x, int y)
{
	overlayX = x;
	overlayY = y;
}







void RXUnitLayerImpl::render(RXUnitRenderer *to,int x,int y, int owner) const
{
	pRender(to, x, y, owner, NULL);
}


void RXUnitLayerImpl::pRender(RXUnitRenderer *to,int x,int y, int owner, const RXUnitLayerImpl *parent) const
{
	for (size_t i=0;i<underlay.size();i++)
		underlay[i]->pRender(to, x, y, owner,this);



	int frame = frameNumber;

	if (frameNumber == 255)
	{
		if(parent)
			frame = parent->frameNumber;
		else
			frame = 0;
	}

	to->blendBitmap(imageID, frame,palette,owner,x+overlayX,y+overlayY+yOffset);

	for (size_t i=0;i<overlay.size();i++)
		overlay[i]->pRender(to, x, y, owner,this);

}




