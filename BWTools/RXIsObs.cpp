#include "stdafx.h"
#include "./RXIsObs.h"
#include "BWlib/BWrepAPI.h"

RXIsObs::RXIsObs(void)
{
}

RXIsObs::~RXIsObs(void)
{
}



bool RXIsObs::IsObs(const BWrepFile &file, const BWrepPlayer &player) 
{

	if (player.isComputer()) 
		return false;

	for (int i = 0; i < file.m_oActions.GetActionCount(); i++)
	{
		const BWrepAction *action = file.m_oActions.GetAction(i);

		if(action != NULL &&
			(action->GetID() == BWrepGameData::CMD_BUILD || action->GetID() == BWrepGameData::CMD_MORPH))
		{
			if (action->GetPlayerID() == player.getSlot()) {
				BWrepActionBuild::Params *params = (BWrepActionBuild::Params *)action->GetParamStruct();
				if(params[0].m_buildingtype != BWrepGameData::BTYP_LAND) 
					return false;
			}
		}
	}

	//Player never build something. We can safely suppose he was an obs
	return true;
}
