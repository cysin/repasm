#ifndef __RX__MATCHUP
#define __RX__MATCHUP

#include <string>
#include "BWTools/RXTeam.h"

class RXPlayer;


class RXMatchup
{
public:
	static std::string getMatchup(RXTeamList &teamList);
	static std::string getMatchup(RXPlayerList &playersList);
};


#endif
