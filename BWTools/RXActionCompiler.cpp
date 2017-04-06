#include "stdafx.h"

//If I include <math.h> the linkers complains about duplicated symbol (cosf and sqrtf in repasm.obj). Don't know why
//By doing so I get things to work, even if this is really a stupid hack. Any explanations welcome
#ifdef STUPID_PHP
extern "C" {
	double floor( double );
}
#else
#include <math.h>
#endif

// stricmp is visual C++ specific, while strcasecmp seems to be gcc specific
// With this quick define we ensure portability on both compilers. If you plan to use
// another compiler, you may have to change this define
#if _MSC_VER > 800
#define strcasecmp _stricmp
#endif


#include "BWlib/BWrepGameData.h"
#include "BWTools/RXActionCompiler.h"
#include "RXPlayerImpl.h"


RXActionCompiler::RXActionCompiler(const BWrepFile &rep, int playerSlot)
{
	player = NULL;
	BWrepPlayer bwPlayer;
	duration = rep.m_oHeader.getGameLength();

	for(int i = 0; i < rep.m_oHeader.getLogicalPlayerCount(); i++)
	{
		rep.m_oHeader.getLogicalPlayers(bwPlayer, i);
		if(bwPlayer.getSlot() == playerSlot)
		{
			player = new RXPlayerImpl(rep, bwPlayer);
			break;
		}
	}

	if(player != NULL)
		getActions(rep);
}

RXActionCompiler::RXActionCompiler(const BWrepFile &rep, const char *playerName)
{
	player = NULL;
	BWrepPlayer bwPlayer;
	duration = rep.m_oHeader.getGameLength();

	for(int i = 0; i < rep.m_oHeader.getLogicalPlayerCount(); i++)
	{
		rep.m_oHeader.getLogicalPlayers(bwPlayer, i);
		if(strcasecmp(bwPlayer.getName(), playerName) == 0)
		{
			player = new RXPlayerImpl(rep, bwPlayer);
			break;
		}
	}

	if(player != NULL)
		getActions(rep);
}

RXActionCompiler::~RXActionCompiler(void)
{
	if(player != NULL)
		delete player;
}


void RXActionCompiler::getLastActions(const BWrepFile &repFile, RXActionsList &list, unsigned short numberOfActions)
{
	list.clear();
	int totActions = repFile.m_oActions.GetActionCount();
	if(totActions < numberOfActions)
		numberOfActions = totActions;

	for(int i = totActions; i > totActions - numberOfActions; i--)
	{
		const BWrepAction *curAction = repFile.m_oActions.GetAction(i - 1);
		list.push_back(*curAction);
	}
}


RXActionsList RXActionCompiler::getActionsForRange(unsigned long startFrame, unsigned long stopFrame, int acType)
{
	RXActionsList result;
	for(unsigned int i = 0; i < actions.size(); i++)
	{
		const BWrepAction curAction = actions[i];
		// We're out of range : stops the processing
		if(curAction.GetTime() > stopFrame && stopFrame > 0)
			break;
		// We're not in range yet ; skips this action
		if(curAction.GetTime() < startFrame)
			continue;

		if(selectAction(curAction, acType))
			result.push_back(curAction);
			//std::cout << "[" << curAction.GetTime() << "]" << "Action " << curAction.GetName() << " by player " << player->getName() << std::endl;
	}

	return result;
}

RXRangedActionsList RXActionCompiler::getActionsAtFrequency(int frequencyInSecs, int acType)
{
	RXRangedActionsList result;
	unsigned long freqTick = BWrepAction::Sec2Tick(frequencyInSecs);
	unsigned long start = 0;

	RXActionsList curRange;
	//std::cout << "Frequency = " << freqTick << std::endl;
	for(unsigned int i = 0; i < actions.size(); i++)
	{
		const BWrepAction curAction = actions[i];
		// We're out of range : pushes the current actionslist into the result and creates another one
		if(curAction.GetTime() > start + freqTick)
		{
			//std::cout << "Action time : " << curAction.GetTime() << " > " << start+freqTick << std::endl;
			if(curRange.size() > 0)
			{
				/*std::cout << "Pushing actions :" << std::endl;
				for(unsigned int j = 0; j < curRange.size(); j++)
					std::cout << "\t[" << curRange[j].GetTime() << "]" << " Action " << curRange[j].GetName() << " by player " << player->getName() << std::endl;*/
				result.push_back(curRange);
				curRange.clear();
			}
			start = (unsigned long)floor((double)curAction.GetTime() / (double)freqTick) * freqTick;
			//std::cout << "New start : " << start << std::endl;
		}
		if(selectAction(curAction, acType))
			curRange.push_back(curAction);
	}
	if(curRange.size() > 0)
		result.push_back(curRange);


	return result;
}

std::vector<int> RXActionCompiler::getAPMWithFrequency(int frequency)
{
	std::vector<int> result;
	unsigned long freqTick = BWrepAction::Sec2Tick(frequency);

	unsigned long lastFrame = freqTick;
	int numAction = 0;

	for(RXActionsList::iterator iter = actions.begin(); iter != actions.end(); iter++)
	{
		const BWrepAction action = *iter;

		if (action.GetPlayerID() == player->getPlayerSlot()) 
		{
			if(action.GetTime() > lastFrame)
			{
				float duration = (float)frequency / 60.0f;
				float apm = (float)numAction / duration;
				result.push_back((int)apm);
				numAction = 0;
				lastFrame += freqTick;
			}
			numAction++;

			if (action.GetID() == BWrepGameData::CMD_LEAVEGAME) //player exited game.
				break;
		}
	}

	// If some actions remain, computes them
	if(numAction > 0)
	{
		float duration = (float)frequency / 60.0f;
		float apm = (float)numAction / duration;
		result.push_back((int)apm);
	}

	return result;
}


void RXActionCompiler::getActions(const BWrepFile &rep)
{
	for(int i = 0; i < rep.m_oActions.GetActionCount(); i++)
	{
		const BWrepAction *curAction = rep.m_oActions.GetAction(i);
		if(curAction->GetPlayerID() == player->getPlayerSlot())
			actions.push_back(*curAction);
	}
}

bool RXActionCompiler::selectAction(const BWrepAction &action, int acType)
{
	if(acType == ALL)
		return true;

	if(acType & BUILD)
	{
		BWrepActionBuild::Params *params = NULL;

		if(action.GetID() == BWrepGameData::CMD_BUILD || action.GetID() == BWrepGameData::CMD_MORPH)
		{
			params = (BWrepActionBuild::Params *)action.GetParamStruct();
			if(params != NULL && params[0].m_buildingtype != BWrepGameData::BTYP_LAND)
				return true;
		}
	}

	if(acType & HOTKEY)
	{
		if(action.GetID() == BWrepGameData::CMD_HOTKEY)
			return true;
	}

	if(acType & RESEARCH)
	{
		if(action.GetID() == BWrepGameData::CMD_RESEARCH)
			return true;
	}

	if(acType & UPGRADE)
	{
		if(action.GetID() == BWrepGameData::CMD_UPGRADE)
			return true;
	}

	return false;
}
