#include "stdafx.h"
#include "RXWinner.h"

#include "BWTools/RXActionCompiler.h"


RXWinner::RXWinner(const BWrepFile &repFile, const RXPlayerList &playerList)
{
	winnerNum = -1;
	probability = 0;
	computeWinner(repFile, playerList);
}

RXWinner::~RXWinner(void)
{
}

void RXWinner::computeWinner(const BWrepFile &repFile, const RXPlayerList &players)
{
	int pos1 = -1, pos2 = -1;
	
	// Gets the indexes in the list for actual players
	for(size_t i = 0; i < players.size(); i++)
	{
		if(players[i]->isHuman() && !players[i]->isObs())
		{
			if(pos1 == -1)
				pos1 = (int)i;
			else if(pos2 == -1)
				pos2 = (int)i;
			// We don't want to find a winner for games other than duels (yet).
			else
				return;
		}
	}

	// Less than 2 players, we don't care
	if(pos1 == -1 || pos2 == -1)
		return;

	unsigned long stamp1 = 0;

	// Easy cases : one or both players left the game at some point
	if(players[pos1]->didLeave())
		stamp1 = players[pos1]->getLeaveStamp();
	if(players[pos2]->didLeave())
	{
		probability = 100;
		if(stamp1 > 0) // both players left the game
		{
			if(stamp1 > players[pos2]->getLeaveStamp()) // player 1 left AFTER player 2, so he is the winner
				winnerNum = pos1;
			else // player 1 left BEFORE player 2, so he is the winner
				winnerNum = pos2;
		}
		else // only player 2 left
			winnerNum = pos1;

		return;
	}
	else if(stamp1 > 0) // only player 1 left
	{
		winnerNum = pos2;
		probability = 100;
		return;
	}

	/* Tough cases : no player left the game.
	 * We are going to determine who won by looking at the last actions each player made.
	 * The winner is usually the one who made the last actions (because the other one is busy typing GG + quitting the game) but that might not be 100% true all the time.
	 * Therefore, the more there are actions made by only one player, the more probable it is that he is the winner.
	 */

	RXActionsList actions;
	// We take 20 more actions because of possible obs actions
	RXActionCompiler::getLastActions(repFile, actions, 40);
	if(actions.size() > 0)
	{
		// First action in list is last action in replay ; gets the owner of it and we have a potential winner
		int lastActionOwner = actions[0].GetPlayerID();
		if(lastActionOwner == players[pos1]->getPlayerSlot())
			winnerNum = pos1;
		else
			winnerNum = pos2;

		int obsActions = 0;
		int playActions = 0;

		// Computes probability
		for(size_t i = 0; i < actions.size(); i++)
		{
			if(playActions == 20)
				break;

			// If the owner of the action is an obs, we wont take the action into account
			if(isObs(actions[i].GetPlayerID(), players))
			{
				obsActions++;
				continue;
			}

			// If the owner is the same as the probable winner, then increment the probability
			if(actions[i].GetPlayerID() == lastActionOwner)
			{
				playActions++;

				// The probability is higher in the beginning because if the 10 last actions were made by only one player, then it is likely he is the winner
				if(i < 5)
					probability += 7;
				else if(i >=5 && i < 13)
					probability += 5;
				else
					probability += 3;
			}
			else
				break;
		}
	}
}

bool RXWinner::isObs(int playerSlot, const RXPlayerList &players)
{
	for(size_t i = 0 ; i < players.size(); i++)
	{
		if(players[i]->getPlayerSlot() == playerSlot)
		{
			if(players[i]->isObs())
				return true;
			else
				return false;
		}
	}

	return false;
}

void RXWinner::getWinner(int &winner, unsigned short &proba)
{
	winner = winnerNum;
	proba = probability;
}
