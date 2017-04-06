#ifndef RX_MAPSECTIONTHG2__H
#define RX_MAPSECTIONTHG2__H

#include "RXMapSection.h"
#include <deque>

class RXMapTHG2Section :
	public RXMapSection
{
public:
	RXMapTHG2Section(std::istream *str, int size);
	RXMapTHG2Section(const unsigned char *data, int size);


	struct RXDoodads
	{
		unsigned short doodadUnit;	//Unit number of the doodad
		unsigned short x;			//X coordinate of doodad UNIT
		unsigned short y;			//Y coordinate of doodad UNIT

		unsigned char owner;		//Player number that owns the doodad
	};

	typedef std::deque<RXDoodads> RXTDoodadsList;

	const RXTDoodadsList & getDoodadsList() const { return list;}

private:

	void createDoodads();
	void getDoodad(RXDoodads &dst, int doodadsNumber);

	RXTDoodadsList list;

	static const int DoodadsSize=10; 
};

#endif




