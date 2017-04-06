#include "stdafx.h"
#include "RXPlayerImpl.h"
#include "BWlib/BWrepAPI.h"

#include "RXColorImpl.h"


RXPlayerImpl::RXPlayerImpl(const BWrepFile &file,  const BWrepPlayer &player)
{
	//Player information
	this->name			= player.getName();
	this->playerId    	= player.getNumber();
	this->playerSlot	= player.getSlot();
	this->bIsHuman      = player.isPlayer();

	//Player Color
	this->color = new RXColorImpl(file, player);

	//Player Race
	switch(player.getRace()) {
		case BWrepPlayer::RACE_ZERG:	race = RXRace::Zerg; break;
		case BWrepPlayer::RACE_TERRAN:  race = RXRace::Terran; break;
		case BWrepPlayer::RACE_PROTOSS: race = RXRace::Protoss; break;
		default:
			race = NULL;
	}

    this->bDidLeave = checkLeave(file, player, leaveStamp);
}


RXPlayerImpl::~RXPlayerImpl(void)
{
}



bool RXPlayerImpl::checkLeave(const BWrepFile &file,  const BWrepPlayer &player, unsigned long &leaveStamp)
{
    leaveStamp = 0;

    for (int i=0;i<file.m_oActions.GetActionCount();i++)
    {
        const BWrepAction *action = file.m_oActions.GetAction(i);

        if (action != NULL && action->GetPlayerID() == player.getSlot()) 
        {

            if (action->GetID() == BWrepGameData::CMD_LEAVEGAME) 
            {
                leaveStamp = BWrepAction::Tick2Sec(action->GetTime()); 
                return true;
            }
        }
    }

    return false;
}
