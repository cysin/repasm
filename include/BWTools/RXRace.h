#ifndef __RX__RACE
#define __RX__RACE

#include <string>
#include <iostream>

class RXRace
{
public:

	enum RaceType 
	{
		RACE_ZERG,
		RACE_TERRAN,
		RACE_PROTOSS
	};

	const std::string &getName()  const { return name;}
	const char         getAbrev() const { return abrev;}
	const RaceType     getRaceID()const { return raceID;}

	void serialize( std::ostream &str) const;
	static const RXRace * import(std::istream &);



	static const RXRace * Zerg;
	static const RXRace * Terran;
	static const RXRace * Protoss;


	static const RXRace * RaceFromID(RaceType type);

protected:
	RXRace(RaceType raceID, const std::string &name, char abrev);
private:
	RaceType raceID;
	std::string name;
	char abrev;


};



#endif
