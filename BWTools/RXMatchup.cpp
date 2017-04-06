#include "stdafx.h"
#include "RXMatchup.h"
#include <sstream>

#include "BWTools/RXPlayer.h"


std::string RXMatchup::getMatchup(RXTeamList &teamList)
{
	std::ostringstream str;

	for (size_t i=0;i<teamList.size();i++)
	{
		RXTeam::const_iterator it  = teamList[i].begin();
		RXTeam::const_iterator end = teamList[i].end();

		for (;it != end; ++it)  
			str<<(*it)->getRace()->getAbrev();

		if (i!=teamList.size()-1)
			str<<'v';
	}

	return str.str();
}

std::string RXMatchup::getMatchup(RXPlayerList &playersList)
{
	std::ostringstream str;
	RXPlayerList nonObs;

	// Lists all non-obs players : computers and humans detected as actual players
	for (size_t i = 0; i < playersList.size(); i++)
	{
		if(playersList[i]->isComputer()
			|| (playersList[i]->isHuman() && !playersList[i]->isObs())
			)
			nonObs.push_back(playersList[i]);
	}

	// Builds the matchup string
	for (size_t i = 0; i < nonObs.size(); i++)
	{
		str << nonObs[i]->getRace()->getAbrev();
		if (i != nonObs.size() - 1)
			str << 'v';
	}

	return str.str();
}
