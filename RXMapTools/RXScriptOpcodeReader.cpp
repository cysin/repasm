#include "stdafx.h"
#include <rxmaptools/RXScriptOpcodeReader.h>

RXScriptOpcodeReader::RXScriptOpcodeReader(void)
{
}

RXScriptOpcodeReader::~RXScriptOpcodeReader(void)
{
}



template<typename T, typename FUNCTION>
void RXScriptOpcodeReader::DispatchEvent(RXScriptOpcodeReaderEvent *event, FUNCTION func, const unsigned char *data)
{
	(event->*func)(*((const T *)data));
}

template<typename T, typename T2, typename FUNCTION>
void RXScriptOpcodeReader::DispatchEvent2(RXScriptOpcodeReaderEvent *event, FUNCTION func, const unsigned char *data)
{
	(event->*func)(*((const T *)data),*((const T2 *)(data+sizeof(T))));
}


template<typename T, typename T2, typename T3,typename FUNCTION>
void RXScriptOpcodeReader::DispatchEvent3(RXScriptOpcodeReaderEvent *event, FUNCTION func, const unsigned char *data)
{
	const T * p = (const T * )data;
	data += sizeof(T);

	const T2 * p2 = (const T2 * )data;
	data += sizeof(T2);

	const T3 * p3 = (const T3 * )data;


	(event->*func)(*p,*p2,*p3);
}

void RXScriptOpcodeReader::read(const unsigned char *script, RXScriptOpcodeReaderEvent *eventDispatch)
{

	while(1)
	{
		RXOpcode op = (RXOpcode)(*script);
		script++;

		switch (op)
		{
		case SET_FRAME:
			eventDispatch->OnSetFrame(script[0]&0xF);	// See the end of http://www.campaigncreations.org/starcraft/stardraft/iscript.shtml
														// Not sure i'im doing the right thing, though
			break;

		case USE_PARENT_FRAME:
			eventDispatch->OnUseParentFrame();
			break;

		case SET_ACTIVE_OVERLAY:
			DispatchEvent3<unsigned short, unsigned char, unsigned char>(eventDispatch, (&RXScriptOpcodeReaderEvent::OnActiveOverlay),script);
			break;
		case SET_ACTIVE_UNDERLAY:
			DispatchEvent3<unsigned short, unsigned char, unsigned char>(eventDispatch, (&RXScriptOpcodeReaderEvent::OnActiveUnderlay),script);
			break;

		case STOP_AND_REMOVEGRAPHICS:
			return;

		default:
			if (isGoto(op))
				return;
		}

		if ((int)op<=0x41) //valid known opcode 
			script += opcodeSize[(int)op]; //skip parameters
	}
}







inline bool RXScriptOpcodeReader::isGoto(RXOpcode opcode)
{
	switch(opcode)
	{
	case GOTO_OFFSET:
	case STOP_AND_REMOVEGRAPHICS:
	case IFUNKNOWN_GOTO1:
	case IF_POWERUP_PICKED_GOTO:
	case IF_TARGET_IN_RANGE:
	case IF_TARGET_ARC:
	case IFUNKNOWN_GOTO2:
		return true;
	}

	return false;
}





//opcode size
const int RXScriptOpcodeReader::opcodeSize[]=
{
	2
		,2
		,2
		,1
		,0
		,1
		,2
		,2
		,4
		,4
		,0
		,2
		,0
		,4
		,0
		,4
		,2
		,4
		,0
		,0
		,0
		,0
		,0
		,0
		,2
		,5
		,4
		,0
		,5
		,0
		,3
		,1
		,1
		,0
		,1
		,0
		,1
		,1
		,0
		,0
		,1
		,1
		,0
		,0
		,0
		,0
		,0
		,0
		,0
		,1
		,0
		,0
		,1
		,2
		,0
		,2
		,1
		,2
		,4
		,6
		,2
		,2
		,0
		,2
		,2
		,1
		,0
};

