#include "stdafx.h"
#include <rxmaptools/RXRenderableUnit.h>

#include <rxmaptools/RXUnitLayer.h>



RXRenderableUnit::RXRenderableUnit(int x, int y, int animationLevel, RXType type, RXUnitLayer *main, int owner,RXUnitLayer *sub)
{
	this->x = x;
	this->y = y;
	this->animationLevel = animationLevel;
	this->type = type;

	this->main = main;
	this->sub1 = sub;
	this->owner = owner;
}

RXRenderableUnit::RXRenderableUnit(const RXRenderableUnit *unit)
{
	int x = 0;

	/*
	this->x				 = unit->x;
	this->y				 = unit->y;
	this->animationLevel = unit->animationLevel;
	this->type			 = unit->type;
	this->owner			 = unit->owner;

	if (unit->main)
		this->main = new RXUnitLayer(unit->main);
	else
		this->main = NULL;

	if (unit->sub1)
		this->sub1 = new RXUnitLayer(unit->sub1);
	else
		this->sub1 = NULL;
	*/
}

RXRenderableUnit::~RXRenderableUnit(void)
{
	if (this->main)
		delete this->main;

	if (this->sub1)
		delete this->sub1;
}






void RXRenderableUnit::render(RXUnitRenderer *target) const
{
	main->render(target,x,y, owner);
	if (sub1)
		sub1->render(target,x,y, owner);
}



















