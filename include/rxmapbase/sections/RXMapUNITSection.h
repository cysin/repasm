#ifndef RX_MAPSECTIONUNIT__H
#define RX_MAPSECTIONUNIT__H

#include "RXMapSection.h"
#include <rxmapbase/RXMapEnum.h>
#include <deque>

class RXMapUNITSection  : public RXMapSection
{
public:
	RXMapUNITSection(std::istream *str, int size);
	RXMapUNITSection(const unsigned char *data, int size);

	struct RXUnit
	{
		unsigned short x;
		unsigned short y;

		RXMapEnum::UnitType unitType;

		unsigned short owner;

		unsigned char hitPoint;
		unsigned char shieldPoint;
		unsigned char energyPoint;

		unsigned int ressourceAmount;
	};


	typedef std::deque<RXUnit> RXTUnitList;

	const RXTUnitList & getUnitList() const { return list;}

private:

	void createUnit();
	void getUnit(RXUnit &dst, int unitNumber);

	static const int UnitSize=36; 

	RXTUnitList list;

};

#endif

