#ifndef RXRENDERABLE_UNIT_CONTAINER_H
#define RXRENDERABLE_UNIT_CONTAINER_H

#include <rxmapbase/RXMapEnum.h>
#include <misc/RXHashmap.h>
#include <deque>

class RXRenderableUnit;
class RXBitmap;

class RXRenderableUnitContainer
{
public:
	RXRenderableUnitContainer(void);
	~RXRenderableUnitContainer(void);


	void addUnit(RXRenderableUnit *unit); //unit will now be owned by the container
	void renderTo(const RXBitmap *dest, float reductionFactor) const;

	int getUnitCount() const { return (int) unitList.size();}
	const RXRenderableUnit * getUnit(int i) const { return unitList[i];}

private:

	class RXTUnitList : public std::deque<RXRenderableUnit *>
	{
	public:
		~RXTUnitList();
	};

	RXTUnitList unitList;
	static bool orderByY(const RXRenderableUnit *a,const RXRenderableUnit *b);
};

#endif
