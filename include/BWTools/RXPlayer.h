#ifndef __RX__PLAYER
#define __RX__PLAYER

#include <string>
#include <iostream>
#include <vector>
#include <stdexcept>
#include "rxmapbase/RX2dCoord.h"
#include "RXRace.h"
#include "BWTools/RXPlayer.h"

class RXColor;


class RXInvalidPlayerException : public std::runtime_error
{
public:
	RXInvalidPlayerException(const char *message);
};


class RXPlayer
{
public:
	~RXPlayer(void);



	int  getPlayerSlot()const				{ return playerSlot;}
	int  getPlayerID()	const				{ return playerId;}
	bool isHuman()		const				{ return bIsHuman;}
	bool isComputer()	const				{ return !bIsHuman;}
	bool isObs()		const				{ return bIsObs;}
	

    bool didLeave() const                  { return bDidLeave;}
    unsigned long getLeaveStamp() const    { return leaveStamp;}

	int getAPM()		const				{ return APM;}
	std::vector<int> getAPMAtFrequency() const;
	
	int getStartPos4()	const				{ return startPos4;}
	int getStartPos12() const				{ return startPos12;}

	const RX2dCoord & getNormalizedPos() const { return normalizedPos;}
	const std::string &getName()		 const { return name;}

	const RXRace  * getRace() const  {return race;}
	const RXColor * getColor() const {return color;}

	void serialize(std::ostream &str);

protected:
	const char * gSerializationVersion;

	RXPlayer(void);

	const RXRace *race;
	RXColor *color;
	
	int playerId;
	int playerSlot;
	std::string name;

	bool bIsHuman;
	bool bIsObs;

    //Did player disconnect ? When ? 
    bool bDidLeave;
    unsigned long leaveStamp;

	int APM;
	int startPos4, startPos12;

	RX2dCoord normalizedPos;
};

#endif
