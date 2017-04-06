#ifndef __RX__PLAYER__IMPL
#define __RX__PLAYER__IMPL

#include "BWTools/RXPlayer.h"

class BWrepFile;
class BWrepPlayer;


class RXPlayerImpl :
	public RXPlayer
{
public:
	RXPlayerImpl(const BWrepFile &file,const BWrepPlayer &player);
	~RXPlayerImpl(void);

	void setStartingPos(int p4, int p12, const RX2dCoord &pos)    
	{
		this->startPos4		= p4;
		this->startPos12	= p12;
		this->normalizedPos = pos;
	}

	void setAPM(int APM)					{ this->APM = APM;}
	void setIsObs(bool isObs)				{this->bIsObs = isObs;}

private:
	bool checkLeave(const BWrepFile &file,  const BWrepPlayer &player, unsigned long &leaveStamp);

};


#endif
