#include "stdafx.h"
#include "BWTools/RXRace.h"

class RXRaceImpl : public RXRace {
public:
	RXRaceImpl(RXRace::RaceType raceID, const std::string &name, char abrev) : RXRace(raceID, name, abrev) {}
};



RXRace::RXRace(RXRace::RaceType id, const std::string &name, char abrev)
{
	this->name  = name;
	this->abrev = abrev;
	this->raceID=id;
}



//Hidden global variable

static RXRaceImpl _Zerg   (RXRace::RACE_ZERG,"Zerg",'Z');
static RXRaceImpl _Terran (RXRace::RACE_TERRAN,"Terran",'T');
static RXRaceImpl _Protoss(RXRace::RACE_PROTOSS,"Protoss",'P');

const RXRace * RXRace::Terran  = &_Terran;
const RXRace * RXRace::Protoss = &_Protoss;
const RXRace * RXRace::Zerg    = &_Zerg;




void RXRace::serialize(std::ostream &str) const {
	str.write((const char *)&raceID, sizeof(raceID));
}


const RXRace * RXRace::import(std::istream &str) {
	RaceType rt;
	str.read((char *)&rt, sizeof(rt));
	return RaceFromID(rt);
}



const RXRace * RXRace::RaceFromID(RaceType type) {
	switch(type) {
		case RACE_ZERG:		return Zerg; 
		case RACE_TERRAN:	return Terran;
		case RACE_PROTOSS:	return Protoss;
		default:
			return NULL;
	}
}
