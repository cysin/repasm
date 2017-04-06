#ifndef __PHP__EXT__STARTING__LOCATION
#define __PHP__EXT__STARTING__LOCATION

#include "BWlib/BWrepMap.h"
#include "rxmapbase/RX2dCoord.h"
#include <vector>

class BWrepFile;
class BWrepPlayer;
class RXMap;


class RXStartingLocation
{
public:

    //NOTE : map size must be multiplied by 32 in order to match (x,y) coordinate space

    //return an 'imprecise' starting location (12,3,6,9)
    static int getStartingPosition4(const BWrepFile &file, const BWrepPlayer &player, RX2dCoord &normCoord);


    //get a more precise starting location using a 12times segmented circle
    static int getStartingPosition12(const BWrepFile &file, const BWrepPlayer &player, RX2dCoord &normCoord);

	static std::vector<int> listStartingPositions12(const BWrepFile &file);
	static std::vector<int> listStartingPositions12(const RXMap *map);

private:

    static const int m_start12[], m_invstart12[];
    static const int m_start4[], m_invstart4[];

    static float getAngleCos(int mapsizeX, int mapSizeY, float x, float y);
    static void  initCosArray(float *cosVal, int numSegment);
	
	static void getStartPos(const BWrepFile &file, const BWrepPlayer &player, int &mx, int &my,int &x, int &y);

	static int computeStartPos12(unsigned short x, unsigned short y, int mapsizeX, int mapsizeY);
};

#endif
