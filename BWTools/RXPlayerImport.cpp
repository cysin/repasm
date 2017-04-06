#include "stdafx.h"
#include "RXPlayerImport.h"
#include "RXColorImpl.h"


RXPlayerImport::RXPlayerImport(std::istream &str) 
{
	std::string header;
    std::getline(str, header, '\0');

    if (header != gSerializationVersion)
        throw RXInvalidPlayerException("Not a valid player (different version)");

	std::getline(str, name, '\0');

	race = RXRace::import(str);
	color = new RXColorImpl(str);

	str.read((char *)&playerId,	sizeof(playerId));
	str.read((char *)&playerSlot,sizeof(playerSlot));
	str.read((char *)&bIsHuman,	sizeof(bIsHuman));
	str.read((char *)&bIsObs,	sizeof(bIsObs));

    str.read((char *)&bDidLeave,	sizeof(bDidLeave));
    str.read((char *)&leaveStamp,	sizeof(leaveStamp));

	str.read((char *)&APM,		sizeof(APM));
	str.read((char *)&startPos4,	sizeof(startPos4));
	str.read((char *)&startPos12,sizeof(startPos12));

	str.read((char *)&normalizedPos.x,	sizeof(normalizedPos.x));
	str.read((char *)&normalizedPos.y,	sizeof(normalizedPos.y));
}

RXPlayerImport::~RXPlayerImport(void)
{
}
