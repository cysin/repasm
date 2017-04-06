#include "stdafx.h"
#include <sstream>
#include <algorithm>
#include "./RXTeamManager.h"
#include "BWTools/RXPlayer.h"

RXTeamManager::RXTeamManager(const BWrepFile &rep, const RXPlayerList &player)
{
	THashMap allied;
	// UMS : doesn't do anything
	if(rep.m_oHeader.getGameType() != BWrepHeader::USE_MAP_SETTINGS)
	{
		// Forces only ; special case of Team Melee : we do not want to take the Ally action into account because of the weird format of the parameters
		if(rep.m_oHeader.getGameType() == BWrepHeader::TEAM_CTF
			|| rep.m_oHeader.getGameType() == BWrepHeader::TEAM_FFA
			|| rep.m_oHeader.getGameType() == BWrepHeader::TOP_VS_BOTTOM
			|| rep.m_oHeader.getGameType() == BWrepHeader::TEAM_MELEE)
		{
			ReadTeamsFromRep(rep, player);
		}

		// Allying only
		else if(rep.m_oHeader.getGameType() == BWrepHeader::GREED
				|| rep.m_oHeader.getGameType() == BWrepHeader::SUDDEN_DEATH
				|| rep.m_oHeader.getGameType() == BWrepHeader::MELEE)
		{
			ExtractTeamsFromRep(rep, player);
			BuildCPUTeam(player);
			AddRemainingPlayers(player);
		}

		// No Force, no Allying
		else
		{
			BuildFFATeams(player);
		}

		cleanTeams();
	}
}

RXTeamManager::~RXTeamManager(void)
{
}


//================ Computes teams in a game where teams are automatically set up ================//
void RXTeamManager::ReadTeamsFromRep(const BWrepFile &rep, const RXPlayerList &playerList)
{
	RXHashNamespace::hash_map<int, RXTeam> teams;

	for(int i = 0; i < rep.m_oHeader.getLogicalPlayerCount(); i++)
	{
		BWrepPlayer curPlayer;
		rep.m_oHeader.getLogicalPlayers(curPlayer, i);
		const RXPlayer * player = playerFromNum(curPlayer.getNumber(), playerList);
		if(!player->isObs())
			teams[curPlayer.getForce()].insert(player);
	}

	RXHashNamespace::hash_map<int, RXTeam>::iterator it;

	for(it = teams.begin();it!=teams.end();++it) {
		teamList.push_back(it->second);
	}
}

//================ Computes teams in a FFA-like game  ================//
void RXTeamManager::BuildFFATeams(const RXPlayerList &playerList)
{
	for(size_t i = 0; i < playerList.size(); i++)
	{
		RXTeam newTeam;
		newTeam.insert(playerList[i]);
		teamList.push_back(newTeam);
	}
}


//================ Computing teams from a melee-type game ================//

// Extract teams by looking at the actions performed during the game
void RXTeamManager::ExtractTeamsFromRep(const BWrepFile &rep, const RXPlayerList &playerList)
{
	THashMap allied;
	ReadAlliances(rep, playerList, allied);

	//dumpAllies(allied);

	THashMap::iterator it;
	BuildTeams(playerList, allied);
}

// Builds the alliances into the hash_map from the Ally actions
void RXTeamManager::ReadAlliances(const BWrepFile &rep, const RXPlayerList &player, THashMap & alliedHmap)
{
	// The action is valid only if it is an Ally one and if it happens before mid-game.
	int allyLimit = (int)(BWrepAction::Tick2Sec(rep.m_oHeader.getGameLength() / 2));

	for(int i = 0; i < rep.m_oActions.GetActionCount(); i++)
	{
		const BWrepAction *action = rep.m_oActions.GetAction(i);

		if(action->GetID() == BWrepGameData::CMD_ALLY && BWrepAction::Tick2Sec(action->GetTime()) < allyLimit)
		{
			const RXPlayer *pl = playerFromSlot(action->GetPlayerID(),player);
			BWrepActionAlly::Params *params = (BWrepActionAlly::Params *)action->GetParamStruct();
			GetAlliesForPlayer(pl, player, params, alliedHmap);
		}
	}
}


void RXTeamManager::dumpAllies(THashMap & alliedHmap)
{
	THashMap::iterator pIter;

	for(pIter = alliedHmap.begin(); pIter != alliedHmap.end(); ++pIter)
	{
		const RXTwoPlayer &key = pIter->first;
		const RXPlayer * leader = key.getP1();
		const RXPlayer * player = key.getP2();

		std::cout << leader->getName().c_str() << " ; " << player->getName().c_str() << std::endl;
	}
}


// Given the parameters of an Ally action, stores the allies of a player into the hash_map
void RXTeamManager::GetAlliesForPlayer(const RXPlayer *player, const RXPlayerList &playerList, BWrepActionAlly::Params *params, THashMap & alliedHmap)
{
	int firstParam = params->m_unknown[0];
	int secondParam = params->m_unknown[1];

	for(int i = 0; i < 8; i++)
	{
		// Taiche assures me this works
		// Of course it does, stfu n00b :o
		// Well I trust you, but please stop obfuscating your code like this ! it breaks my nerve
		int test = 0;
		if(i < 4)
			test = firstParam & (2 << 2*i);
		else
			test = secondParam & (2 << 2* (i - 4));

		if(test)
		{
			const RXPlayer *curPlayer = playerFromNum(i, playerList);
			if(NULL != curPlayer)
			{
				if(player->getPlayerID() != curPlayer->getPlayerID())
				{
					RXTwoPlayer players(player, curPlayer);
					alliedHmap[players]++;
				}
			}
		}
	}
}




// Once the actions were parsed and the hash_map is filled, computes the team according to the alliances.
// This method may not be the best in terms of performance, but it proves solid and works for most (all ?) cases
void RXTeamManager::BuildTeams(const RXPlayerList &playerList, THashMap & alliedHmap)
{
	for(size_t i = 0; i < playerList.size(); i++)
	{
		const RXPlayer *player1 = playerList[i];
		if (player1->isComputer() || player1->isObs())
			continue;

		// We want to see who this player is allied with : lists all other players and looks if they're allied with the current player
		for(size_t j = 0; j < playerList.size(); j++)
		{
			const RXPlayer *player2 = playerList[j];
			// Computer or same player
			if (player2->isComputer() || player2->isObs() || j == i)
				continue;

			RXTwoPlayer normal(player1, player2);
			RXTwoPlayer reverse(player2, player1);
			// If both players are allied...
			if(alliedHmap[normal] >= 1 && alliedHmap[reverse] >= 1)
			{
				// ...looks if they are already in the same team
				bool isSameTeam = false;
				std::vector<int> teams = GetTeamsOfPlayer(player1);
				for(size_t k = 0; k < teams.size(); k++)
				{
					if(IsInTeam(player2, teams[k]))
					{
						isSameTeam = true;
						break;
					}
				}

				// If they are in the same team, no need to do anything : next player please !
				if(isSameTeam)
					continue;

				// Not the same team : for each team player1 belongs to, we need to know if playerList[j] can fit in as well
				for(size_t k = 0; k < teams.size(); k++)
				{
					if(IsAlliedWithAllMembers(player2, teamList[teams[k]], alliedHmap))
					{
						teamList[teams[k]].insert(player2);
						isSameTeam = true;
					}
				}

				// If the player was added to a team, it's over
				if(isSameTeam)
					continue;

				// Otherwise, we need to create a new team
				RXTeam newTeam;
				newTeam.insert(player1);
				newTeam.insert(player2);
				teamList.push_back(newTeam);
			}
		}
	}
}


// Puts all CPU players in the same team
void RXTeamManager::BuildCPUTeam(const RXPlayerList &playerList)
{
	RXTeam teamCPU;
	for(size_t i = 0; i < playerList.size(); i++)
	{
		if (playerList[i]->isComputer())
			teamCPU.insert(playerList[i]);
	}
	if(teamCPU.size() > 0)
		teamList.push_back(teamCPU);
}

// All players that didn't find a team will get one of their own ; this is for matchup computation only, since cleanTeams() will normally remove these teams.
void RXTeamManager::AddRemainingPlayers(const RXPlayerList &playerList)
{
	for(size_t i = 0; i < playerList.size(); i++)
	{
		if (GetTeamsOfPlayer(playerList[i]).size() == 0 && playerList[i]->isObs() ==false) {
			RXTeam newTeam;
			newTeam.insert(playerList[i]);
			teamList.push_back(newTeam);
		}
	}
}


// Removes all teams containing only one player
void RXTeamManager::cleanTeams() {
	for(size_t i = 0; i < teamList.size(); i++)
	{
		if (teamList[i].size()>1)
			return;
	}

	teamList.clear();
}


inline const RXPlayer * RXTeamManager::playerFromNum(int playerNum, const RXPlayerList &playerList) {
	for (size_t i = 0; i < playerList.size(); i++) 
		if (playerNum == playerList[i]->getPlayerID())
			return playerList[i];

	return NULL;
}

inline const RXPlayer * RXTeamManager::playerFromSlot(int playerSlot, const RXPlayerList &playerList) {
	for (size_t i=0;i<playerList.size();i++) 
		if (playerSlot == playerList[i]->getPlayerSlot())
			return playerList[i];

	return NULL;
}





std::vector<int> RXTeamManager::GetTeamsOfPlayer(const RXPlayer *player)
{
	std::vector<int> res;
	for(unsigned int i = 0; i < teamList.size(); i++)
	{
		if(IsInTeam(player, i))
			res.push_back(i);
	}
	return res;
}

bool RXTeamManager::IsInTeam(const RXPlayer *player, int teamNumber)
{

	RXTeam::iterator it = teamList[teamNumber].begin();

	while(it != teamList[teamNumber].end())
	{
		if ((*it)->getPlayerID() == player->getPlayerID())
			return true;

		++it;
	}

	return false;
}




bool RXTeamManager::IsAlliedWithAllMembers(const RXPlayer *player, RXTeam &team, THashMap &alliedHmap)
{

	for (RXTeam::iterator it = team.begin(); it != team.end(); ++it) {
		RXTwoPlayer curPair(player, *it);
		RXTwoPlayer reversePair(*it, player);
		if(alliedHmap[curPair] == 0 || alliedHmap[reversePair] == 0)
			return false;
	}

	return true;
}


void RXTeamManager::serialize(std::ostream &str, const RXTeamList &tl) {
	
	unsigned char size = (unsigned char)tl.size(); 
	str.write((const char *)&size, sizeof(size));

	for (unsigned char i=0;i<size;i++) {
		unsigned char pSize = (unsigned char)tl[i].size(); 
		str.write((const char *)&pSize, sizeof(pSize));

		RXTeam::const_iterator it = tl[i].begin();
		while(it != tl[i].end()) {
			int id = (*it)->getPlayerID();
			str.write((const char *)&id, sizeof(id));

			++it;
		}
	}
}


RXTeamList RXTeamManager::import(std::istream &str, const RXPlayerList &pl){

	RXTeamList res;

	unsigned char size;
	str.read((char *)&size, sizeof(size));

	for (unsigned char i=0;i<size;i++) {
		RXTeam team;
		unsigned char pSize;
		str.read((char *)&pSize, sizeof(pSize));

		for (unsigned char j=0;j<pSize;j++) {
			int id;
			str.read((char *)&id, sizeof(id));
			team.insert(playerFromNum(id, pl));
		}
		res.push_back(team);
	}

	return res;

}












