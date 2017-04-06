#ifndef __RX__TEAM
#define __RX__TEAM

#include <vector>
#include <set>

class RXPlayer;

typedef std::vector<RXPlayer *>	RXPlayerList;
typedef std::set<const RXPlayer *>		RXTeam;
typedef std::vector< RXTeam >			RXTeamList;


#endif
