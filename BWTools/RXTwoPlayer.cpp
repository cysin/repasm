#include "stdafx.h"
#include <BWTools/RXPlayer.h>
#include "./RXTwoPlayer.h"


RXTwoPlayer::RXTwoPlayer(const RXPlayer * _p1, const RXPlayer *_p2) 
{
	p1 = _p1;
	p2 = _p2;
}

  
RXTwoPlayer::operator size_t() const 
{ 
	return (size_t)(p1->getPlayerID() | (p2->getPlayerID()<<16));
};  



/*
bool RXTwoPlayer::operator < (const RXTwoPlayer &r) const {
	if (p1->getPlayerID() < r.p1->getPlayerID()) 
		return true;
	else if (p1->getPlayerID() == r.p1->getPlayerID() && p2->getPlayerID() < r.p2->getPlayerID()) 
		return true;
	
	return false;  
}
*/
