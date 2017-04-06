#include "stdafx.h"
#include <rxmaptools/RXRenderableUnitContainer.h>
#include <rxmaptools/RXRenderableUnit.h>
#include <algorithm>

RXRenderableUnitContainer::RXTUnitList::~RXTUnitList()
{
	for (size_t i=0;i<this->size();i++)
		delete (*this)[i];
}



RXRenderableUnitContainer::RXRenderableUnitContainer(void)
{
}

RXRenderableUnitContainer::~RXRenderableUnitContainer(void)
{
}



bool RXRenderableUnitContainer::orderByY(const RXRenderableUnit *a,const RXRenderableUnit *b)
{
	if (a->getAnimationLevel() < b->getAnimationLevel())
		return true;
	
	if (a->getAnimationLevel() > b->getAnimationLevel())
		return false;


	return a->getY() < b->getY();

}





void RXRenderableUnitContainer::addUnit(RXRenderableUnit *unit)
{
	unitList.push_back(unit);

	std::sort(unitList.begin(), unitList.end(), orderByY);
}










