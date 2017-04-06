#include "stdafx.h"
#include "BWTools/RXPlayer.h"
#include "BWTools/RXColor.h"


RXInvalidPlayerException::RXInvalidPlayerException(const char *message)
: std::runtime_error(std::string("Invalid player : ") + message)
{
}


RXPlayer::RXPlayer(void)
{
	// When adding/removing/modifying serialized data, change the version here.
	// This will force the repcache files to update (instead of... crashing explorer.exe :p)
	gSerializationVersion = "RXP01";

	//Set default value
	color = NULL;
	race  = NULL;


	playerId = playerSlot = 0;
	name = "unnamed";


	bIsHuman = false;
	bIsObs   = false;
    bDidLeave = false;

	leaveStamp = 0;

	APM = 0;

	startPos4 = startPos12 = 0;

	normalizedPos.x = normalizedPos.y = 0;
}

RXPlayer::~RXPlayer(void)
{
	if (color)
		delete color;
}



void RXPlayer::serialize(std::ostream &str)
{
	str << gSerializationVersion<<'\0';

	str << name << '\0';

	race->serialize(str);
	color->serialize(str);

	str.write((const char *)&playerId,	sizeof(playerId));
	str.write((const char *)&playerSlot,sizeof(playerSlot));
	str.write((const char *)&bIsHuman,	sizeof(bIsHuman));
	str.write((const char *)&bIsObs,	sizeof(bIsObs));

    str.write((const char *)&bDidLeave,	sizeof(bDidLeave));
    str.write((const char *)&leaveStamp,	sizeof(leaveStamp));

    
	str.write((const char *)&APM,		sizeof(APM));
	str.write((const char *)&startPos4,	sizeof(startPos4));
	str.write((const char *)&startPos12,sizeof(startPos12));

	str.write((const char *)&normalizedPos.x,	sizeof(normalizedPos.x));
	str.write((const char *)&normalizedPos.y,	sizeof(normalizedPos.y));

}










