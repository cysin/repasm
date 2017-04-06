#ifndef __RX__TEAM__MANAGER
#define __RX__TEAM__MANAGER

#include <iostream>
#include <vector>
#include <set>
#include <BWlib/BWrepAPI.h>
#include "RXTwoPlayer.h"

#include "BWTools/RXTeam.h"
#include "misc/RXHashmap.h"


class RXTeamManager
{
public:
	RXTeamManager(const BWrepFile &rep, const RXPlayerList &player);
	~RXTeamManager(void);

	RXTeamList  getTeamList()		 const {return teamList;}

	static void		  serialize(std::ostream &str, const RXTeamList &tl);
	static RXTeamList import(std::istream &str, const RXPlayerList &pl);


private:
	typedef RXHashNamespace::hash_map<RXTwoPlayer, int> THashMap;

	void ReadTeamsFromRep(const BWrepFile &rep, const RXPlayerList &playerList);
	void ExtractTeamsFromRep(const BWrepFile &rep, const RXPlayerList &playerList);
	void BuildFFATeams(const RXPlayerList &playerList);
	void ReadAlliances(const BWrepFile &rep, const RXPlayerList &player, THashMap & alliedHmap);
	void BuildTeams(const RXPlayerList &playerList, THashMap & alliedHmap);
	void BuildCPUTeam(const RXPlayerList &playerList);
	void AddRemainingPlayers(const RXPlayerList &playerList);


	void GetAlliesForPlayer(const RXPlayer *player, const RXPlayerList &playerList, BWrepActionAlly::Params *params, THashMap & alliedHmap);
	std::vector<int>  GetTeamsOfPlayer(const RXPlayer *player);
	bool IsInTeam(const RXPlayer *player, int teamNumber);
	bool IsAlliedWithAllMembers(const RXPlayer *player, RXTeam &team, THashMap &alliedHmap);

	static inline const RXPlayer * playerFromSlot(int playerSlot, const RXPlayerList &playerList);
	static inline const RXPlayer * playerFromNum(int playerNum, const RXPlayerList &playerList);

	void cleanTeams();

	void dumpAllies(THashMap & alliedHmap);

	RXTeamList teamList;
};

#endif
