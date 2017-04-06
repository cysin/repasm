#include "stdafx.h"
#include "BWlib/BWrepAPI.h"
#include "./RXAPM.h"


int RXAPM::compute(const BWrepFile &file, const BWrepPlayer &player)
{

	int maxDuration = file.m_oHeader.getGameLength();
    int numAction=0;

	unsigned long minTicks = BWrepAction::Sec2Tick(120); //calcul a la BWChart

    for (int i=0;i<file.m_oActions.GetActionCount();i++)
    {
        const BWrepAction *action = file.m_oActions.GetAction(i);

		if (action != NULL && action->GetPlayerID() == player.getSlot()) 
		{
            if (action->GetTime() >= minTicks)
                numAction++;

			if (action->GetID() == BWrepGameData::CMD_LEAVEGAME) //player exited game. 
				maxDuration = action->GetTime();
		}
    }

	int duration = BWrepAction::Tick2Sec(maxDuration) - 120;

	if (duration <= 0)
		return 0 ;


    float apm = (float)numAction / ((float)(duration)/60.0f);

    return (int)apm;
}
