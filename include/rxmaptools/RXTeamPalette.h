
#ifndef __RX__TEAMPALETTE
#define __RX__TEAMPALETTE

#include <rxmaptools/RXPalette.h>

class RXTeamPalette :
	public RXPalette
{
public:
	RXTeamPalette(const RXPalette &src, int teamNumber);
	~RXTeamPalette(void);

	static const unsigned int *getTeamColor(int teamNumber);

private:
	static const unsigned int teamColor[128];
};


#endif 
