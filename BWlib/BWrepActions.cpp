//----------------------------------------------------------------------------------------------------
// Replay Actions - jca (May 2003)
//----------------------------------------------------------------------------------------------------
#include "stdafx.h"
#include "BWlib/BWrepAPI.h"
#include "BWlib/BWrepActions.h"
#include "BWlib/BWrepGameData.h"
#include "unpack.h"
//#include <assert.h>

//------------------------------------------------------------------------------------------------------------

// to convert ticks to seconds
const float BWrepAction::m_timeRatio = 23.8f; 

int BWrepAction::Tick2Sec(unsigned long tick)
{
	return (int)((float)tick/(float)m_timeRatio);
}

// convert seconds to tick
unsigned long BWrepAction::Sec2Tick(int sec)
{
	return (unsigned long)((float)sec*(float)m_timeRatio);
}

//------------------------------------------------------------------------------------------------------------

#define CONSUME(type) *((type*)current); current+=sizeof(type); read+=sizeof(type);
#define CONSUMES(type) *((type*)current); current+=sizeof(type); read+=sizeof(type); size-=sizeof(type);
#define CONSUMEB(bytes) current+=bytes; read+=bytes;
#define CONSUMEBS(bytes) current+=bytes; read+=bytes; size-=bytes;
#define ASSIGNCLASS(classname) m_pGetParamText = &BWrepAction##classname::gGetParameters; m_datasize=sizeof(BWrepAction##classname::Params); CONSUMEBS(m_datasize); return true;
#define ASSIGNCLASS_NP(classname) m_pGetParamText = &BWrepAction##classname::gGetParameters; m_datasize=0; return true;
#define ASSIGNCLASSV(classname,bytes) m_pGetParamText = &BWrepAction##classname::gGetParameters; m_datasize=bytes; CONSUMEBS(m_datasize); return true;

bool BWrepAction::ProcessActionParameters(const unsigned char * &current, int& read, unsigned char& size)
{
	switch(m_ordertype)
	{
	case 0x00:	//0x00
	case 0x01:
	case 0x02:
	case 0x03:
	case 0x04:
	case 0x05:	//0x05
	case 0x06:
	case 0x07:
	case 0x08:
		break;
	case 0x09: 
		{
			//select
			int bytes = 1+((int)*current)*sizeof(unsigned short);
			ASSIGNCLASSV(Select,bytes);
		}
	case 0x0A: 
		{
			//shift select
			int bytes = 1+((int)*current)*sizeof(unsigned short);
			ASSIGNCLASSV(ShiftSelect,bytes);
		}
	case 0x0B:
		{
			//shift deselect
			int bytes = 1+((int)*current)*sizeof(unsigned short);
			ASSIGNCLASSV(ShiftDeselect,bytes);
		}
	case 0x0C: ASSIGNCLASS(Build);
	case 0x0D: ASSIGNCLASS(Vision);
	case 0x0E: ASSIGNCLASS(Ally);
	case 0x0f: // Change Game Speed (Taiche)
	case 0x10:	//0x10
	case 0x11:
	case 0x12:
		break;
	case 0x13: ASSIGNCLASS(HotKey);
	case 0x14: ASSIGNCLASS(Move);
	case 0x15: ASSIGNCLASS(Attack);
	case 0x16:
	case 0x17:
		break;
	case 0x18: ASSIGNCLASS_NP(Cancel);
	case 0x19: ASSIGNCLASS_NP(CancelHatch);
	case 0x1A: ASSIGNCLASS(Stop);
	case 0x1b:
	case 0x1c:
	case 0x1d:
		break;
	case 0x1E: ASSIGNCLASS(ReturnCargo);
	case 0x1F: ASSIGNCLASS(Train);
	case 0x20: ASSIGNCLASS(CancelTrain);
	case 0x21: ASSIGNCLASSV(Cloak,size);
	case 0x22: ASSIGNCLASSV(Decloak,size);
	case 0x23: ASSIGNCLASS(Hatch);
	case 0x24:
		break;
	case 0x25: ASSIGNCLASS(Unsiege);
	case 0x26: ASSIGNCLASS(Siege);
	case 0x27: ASSIGNCLASS_NP(BuildInterceptor);
	case 0x28: ASSIGNCLASS(UnloadAll);
	case 0x29: ASSIGNCLASS(Unload);
	case 0x2A: ASSIGNCLASS_NP(MergeArchon);
	case 0x2B: ASSIGNCLASS(HoldPosition);
	case 0x2C: ASSIGNCLASS(Burrow);
	case 0x2D: ASSIGNCLASS(Unburrow);
	case 0x2e: ASSIGNCLASS_NP(CancelNuke);
	case 0x2f: ASSIGNCLASS(Lift);
	case 0x30: ASSIGNCLASS(Research);
	case 0x31: ASSIGNCLASS_NP(CancelResearch);
	case 0x32: ASSIGNCLASS(Upgrade);
	case 0x33:
	case 0x34:
		break;
	case 0x35: ASSIGNCLASS(Morph);
	case 0x36: ASSIGNCLASS_NP(Stimpack);
	case 0x37:
	case 0x38:
	case 0x39:
	case 0x3a:	//0x3a
	case 0x3b:
	case 0x3c:
	case 0x3d:
	case 0x3e:
	case 0x3f:
	case 0x40:	//0x40
	case 0x41:
	case 0x42:
	case 0x43:
	case 0x44:
	case 0x45:	//0x45
	case 0x46:
	case 0x47:
	case 0x48:
	case 0x49:
	case 0x4a:	//0x4a
	case 0x4b:
	case 0x4c:
	case 0x4d:
	case 0x4e:
	case 0x4f:
	case 0x50:	//0x50
	case 0x51:
	case 0x52:
	case 0x53:
	case 0x54:
	case 0x55:
	case 0x56:
		break;
	case 0x57:ASSIGNCLASS(LeftGame);
	case 0x58:
	case 0x59:
		break;
	case 0x5A:ASSIGNCLASS_NP(MergeDarkArchon);
	case 0x5B:
		break;
	case 0x5C:
		ASSIGNCLASS(Chat);
	default:
		//assert(0);
		break;
	}

	//assert(m_ordertype==0x33 || m_ordertype==0x34 || m_ordertype==0x0F || m_ordertype==0x12);
	ASSIGNCLASSV(Unknown,size);
	return false;
}

//------------------------------------------------------------------------------------------------------------

static const char *_MkUnitID(char *buffer, unsigned short unitid, short objid)
{
	if(objid!=-1)
		sprintf(buffer,"%s",BWrepGameData::GetObjectNameFromID(objid));
	else
		sprintf(buffer,"%d",(int)unitid);
	return buffer;
}

const char *BWrepAction::MkUnitID2String(char *buffer, unsigned short unitid, IUnitIDToObjectID *interf, unsigned long time)
{
	if(unitid==0) {strcpy(buffer,"0"); return buffer;}
	short objid=interf==0?-1:interf->Convert(unitid,time);
	return _MkUnitID(buffer, unitid, objid);
}

static const char *_MkUnitList(int unitcount, unsigned short *unitid, IUnitIDToObjectID *interf, unsigned long time)
{
	static char gszUnits[1024];
	short objid[12];

	if(unitcount==0) {strcpy(gszUnits,"(lost vision on selected unit)"); return gszUnits;}

	assert(unitcount<=12);
	if(unitcount>12) unitcount=12;

	gszUnits[0]=0;

	bool allsame=true;
	for(int i=0; i<unitcount; i++)
	{
		objid[i]=interf==0 ? -1 : interf->Convert(unitid[i],time);
		if(i>0 && objid[i]!=objid[i-1]) allsame=false;
	}

	if(allsame && objid[0]!=-1)
	{
		sprintf(gszUnits,"%s",BWrepGameData::GetObjectNameFromID(objid[0]));
		if(unitcount>1)  sprintf(&gszUnits[strlen(gszUnits)],"(x%d)",unitcount);
	}
	else
	{
		for(int i=0; i<unitcount; i++)
		{
			if(i!=0) strcat(gszUnits,",");
			_MkUnitID(&gszUnits[strlen(gszUnits)], unitid[i], objid[i]);
		}	
	}

	strcat(gszUnits," [");

	for(int i=0; i<unitcount; i++)
	{
		if(i!=0) strcat(gszUnits,",");
		sprintf(&gszUnits[strlen(gszUnits)],"%d",(int)unitid[i]);
	}	

	//strcat(gszUnits,"]");

	return gszUnits;
}

//------------------------------------------------------------------------------------------------------------

#define IMPLACTION(classname,bufsize) const char *BWrepAction##classname::gGetParameters(const BWrepAction *action, const unsigned char *data, int datasize,IUnitIDToObjectID *interf){ \
    static char gszParams[bufsize];	\
	gszParams[0]=0; \
	BWrepAction##classname::Params *p = (BWrepAction##classname::Params*)data;
#define ENDACTION return gszParams;}

//------------------------------------------------------------------------------------------------------------

#define IMPLACTION_NOPARAM(classname) const char *BWrepAction##classname::gGetParameters(const BWrepAction *action, const unsigned char *data, int datasize,IUnitIDToObjectID *interf){ \
return "";}

// "stop" action
IMPLACTION(Stop,8)
	sprintf(gszParams,"%d",(int)p->m_unknown);
ENDACTION

// "select" action
IMPLACTION(Select,128)
	strcpy(gszParams,_MkUnitList(p->m_unitCount, p->m_unitid, interf, action->GetTime()));
ENDACTION

// "deselect" action
IMPLACTION(Deselect,128)
	strcpy(gszParams,_MkUnitList(p->m_unitCount, p->m_unitid, interf, action->GetTime()));
ENDACTION

// "shift select" action
/*
IMPLACTION(ShiftSelect,128)
	strcpy(gszParams,_MkUnitList(p->m_unitCount, p->m_unitid, interf, action->GetTime()));
ENDACTION*/

// "shift deselect" action
IMPLACTION(ShiftDeselect,128)
	strcpy(gszParams,_MkUnitList(p->m_unitCount, p->m_unitid, interf, action->GetTime()));
ENDACTION

// "train" action
IMPLACTION(Train,64)
	assert(p->m_unitType<BWrepGameData::g_ObjectsSize);
	strcpy(gszParams,BWrepGameData::g_Objects[p->m_unitType]);
ENDACTION

// "hatch" action
IMPLACTION(Hatch,64)
	assert(p->m_unitType<BWrepGameData::g_ObjectsSize);
	strcpy(gszParams,BWrepGameData::g_Objects[p->m_unitType]);
ENDACTION

// "cancel train" action
IMPLACTION(CancelTrain,64)
	sprintf(gszParams,"%02X %02X",(int)p->m_unknown[0],(int)p->m_unknown[1]);
ENDACTION

// "move" action
IMPLACTION(Move,32)
	char szUnit[64];
	MkUnitID2String(szUnit, p->m_unitid, interf,action->GetTime());
	if(p->m_unknown2==1)
		sprintf(gszParams,"(%d,%d),%s,%d,%s [%d",(int)p->m_pos1,(int)p->m_pos2,szUnit,(int)p->m_unknown1,BWrepGameData::g_AttackModifiers[p->m_unknown2],p->m_unitid);
	else		
		sprintf(gszParams,"(%d,%d),%s,%d,%d [%d",(int)p->m_pos1,(int)p->m_pos2,szUnit,(int)p->m_unknown1,(int)p->m_unknown2,p->m_unitid);
ENDACTION

// "build" action
IMPLACTION(Build,128)
	assert(p->m_buildingid<BWrepGameData::g_ObjectsSize);
	assert(p->m_buildingtype<BWrepGameData::g_BuildingTypesSize);
	sprintf(gszParams,"%s,(%d,%d),%s",BWrepGameData::g_BuildingTypes[p->m_buildingtype],(int)p->m_pos1,(int)p->m_pos2,BWrepGameData::g_Objects[p->m_buildingid]);
ENDACTION

// "research" action
IMPLACTION(Research,64)
	assert(p->m_techid<BWrepGameData::g_ResearchSize);
	strcpy(gszParams,BWrepGameData::g_Research[p->m_techid]);
ENDACTION

// "upgrade" action
IMPLACTION(Upgrade,64)
	assert(p->m_upgid<BWrepGameData::g_UpgradesSize);
	strcpy(gszParams,BWrepGameData::g_Upgrades[p->m_upgid]);
ENDACTION

// "lift" action
IMPLACTION(Lift,32)
	sprintf(gszParams,"%02X %02X %02X %02X",(int)p->m_unknown[0],(int)p->m_unknown[1],(int)p->m_unknown[2],(int)p->m_unknown[3]);
ENDACTION

// "attack" action
IMPLACTION(Attack,128)
	assert(p->m_type<BWrepGameData::g_AttacksSize);
	assert(p->m_modifier<BWrepGameData::g_AttackModifiersSize);
	//const char *attack = BWrepGameData::g_Attacks[p->m_type];
	//if(attack[0]==0) attack="Unknown";
	char szUnit[64];
	MkUnitID2String(szUnit, p->m_unitid, interf, action->GetTime());
	sprintf(gszParams,"(%d,%d),%s,%d,%s [%d",(int)p->m_pos1,(int)p->m_pos2,szUnit,(int)p->m_unknown1,BWrepGameData::g_AttackModifiers[p->m_modifier],p->m_unitid);//,attack);
ENDACTION

// "ally" action
IMPLACTION(Ally,32)
	sprintf(gszParams,"%02X %02X %02X %02X",(int)p->m_unknown[0],(int)p->m_unknown[1],(int)p->m_unknown[2],(int)p->m_unknown[3]);
ENDACTION

// "vision" action
IMPLACTION(Vision,16)
	sprintf(gszParams,"%02X %02X",(int)p->m_unknown[0],(int)p->m_unknown[1]);
ENDACTION

// "hotkey" action
IMPLACTION(HotKey,64)
	assert(p->m_type<BWrepGameData::g_HotKeyModifiersSize);
	int slot = p->m_slot;
	if(slot==0) slot=10;
	sprintf(gszParams,"%s,%d",BWrepGameData::g_HotKeyModifiers[p->m_type],slot);
ENDACTION

// "holdposition" action
IMPLACTION(HoldPosition,8)
	sprintf(gszParams,"%02X",(int)p->m_unknown);
ENDACTION

// "unknown" action
IMPLACTION(Unknown,256)
	for(int i=0; i<datasize; i++)
	{
		if(i!=0) strcat(gszParams," ");
		sprintf(&gszParams[strlen(gszParams)],"%02X",(int)p->m_unknown[i]);
	}
ENDACTION

// "cloak" action
IMPLACTION(Cloak,128)
	return BWrepActionUnknown::gGetParameters(action,data,datasize,interf);
ENDACTION

// "decloak" action
IMPLACTION(Decloak,128)
	return BWrepActionUnknown::gGetParameters(action,data,datasize,interf);
ENDACTION

// "siege" action
IMPLACTION(Siege,128)
	sprintf(gszParams,"%02X",(int)p->m_unknown[0]);
ENDACTION

// "unsiege" action
IMPLACTION(Unsiege,128)
	sprintf(gszParams,"%02X",(int)p->m_unknown[0]);
ENDACTION

// "cancel building" action
IMPLACTION_NOPARAM(Cancel)

// "cancel nuke" action
IMPLACTION_NOPARAM(CancelNuke)

// "cancel hatch" action
IMPLACTION_NOPARAM(CancelHatch)

// "cancel research" action
IMPLACTION_NOPARAM(CancelResearch)

// "stimpack" action
IMPLACTION_NOPARAM(Stimpack)

// "Build Interceptor/scarab" action
IMPLACTION(BuildInterceptor,64)
	strcpy(gszParams,"Interceptor/Scarab");
ENDACTION

// "merge archon" action
IMPLACTION_NOPARAM(MergeArchon)

// "merge dark archon" action
IMPLACTION_NOPARAM(MergeDarkArchon)

// "unload" action
IMPLACTION(Unload,32)
	sprintf(gszParams,"%02X %02X",(int)p->m_unknown[0],(int)p->m_unknown[1]);
ENDACTION

// "unload all" action
IMPLACTION(UnloadAll,8)
	sprintf(gszParams,"%02X",(int)p->m_unknown[0]);
ENDACTION

// "return cargo" action
IMPLACTION(ReturnCargo,8)
	sprintf(gszParams,"%02X",(int)p->m_unknown[0]);
ENDACTION

// "leftgame" action
IMPLACTION(LeftGame,16)
	if(p->m_how==1)
		strcpy(gszParams,"player quit");
	else if(p->m_how==6)
		strcpy(gszParams,"player dropped");
	else
		sprintf(gszParams,"%02X",(int)p->m_how);
ENDACTION

// "morph" action
IMPLACTION(Morph,64)
	assert(p->m_buildingid<BWrepGameData::g_ObjectsSize);
	sprintf(gszParams,"%s",BWrepGameData::g_Objects[p->m_buildingid]);
ENDACTION

// "burrow" action
IMPLACTION(Burrow,8)
	sprintf(gszParams,"%02X",(int)p->m_unknown[0]);
ENDACTION

// "unburrow" action
IMPLACTION(Unburrow,8)
	sprintf(gszParams,"%02X",(int)p->m_unknown[0]);
ENDACTION

// "unsiege" action
IMPLACTION(Chat,256)
	sprintf(gszParams,"%s",data);
ENDACTION


//------------------------------------------------------------------------------------------------------------

// <buffer>           ::= {<time param><block size>{<actions>}}
// <time param>       ::= 4 byte time value indicating offset from beginning of games (=time)
// <block size>       ::= 1 byte size of all actions in that time tick
// <action>           ::= <player id><order type><order param list>
// <player id>        ::= 1 byte player id found in the header in section 1
// <order type>       ::= 1 byte id of the type of order
// <order param list> ::= number, format and meaning determined by order type;
//
bool BWrepActionList::DecodeActions(const unsigned char *buffer, int cmdSize, bool clear)
{
	const unsigned char *current=0;
	const unsigned char *begin=0;
	unsigned long off;
	int read=0;
	BWrepAction action;

	// if we start anew
	if(clear) 
	{
		// keep pointer on the buffer we're given
		_Clear();
		m_datasize = cmdSize;
		m_data = (unsigned char *)buffer;
		current = buffer;
		off=0;
	}
	else
	{
		// grow current buffer & append the new data
		m_data = (unsigned char *)realloc((void*)m_data,m_datasize+cmdSize);
		memcpy(m_data+m_datasize,buffer,cmdSize);
		current = m_data+m_datasize;
		off = m_datasize;
		m_datasize+=cmdSize;
		free((void*)buffer);
	}

	begin = current;
	unsigned long lastTime = 0;
	while(read<cmdSize)
	{
		// get time
		unsigned long time = (unsigned long)CONSUME(unsigned int);

		// any hint to a corrupted replay?
		if(time - lastTime > 10000) 
			return false;

		// remember time
		lastTime = time;

		//assert(time!=5530);

		// get block size (a block can contain multiple actions for the same time tick)
		byte size = CONSUME(byte);
		while(size>0)
		{
			// start new action
			action.Clear();

			// set action time
			action.SetTime(time);

			// get player id
			byte playerid = CONSUMES(byte);
			action.SetPlayerID(playerid);

			// get order id
			byte orderid = CONSUMES(byte);
			action.SetOrderType(orderid);

			//assert(orderid!=0x21);

			// keep pointer on action data
			action.SetData(this, off + (current-begin));

			// get action parameters (depending on orderid)
			if(action.ProcessActionParameters(current, read, size))
			{
				// add action
				if(!AddAction(&action))
					return false;
				//OutputDebugString(action.GetParameters());
				//OutputDebugString("\r\n");
			}
			else
			{
				// we dont know how to handle an action, skip remaining bytes for that time tick
				// all remaining actions for that time tick are lost
				assert(0);
				break;
			}
		}

		CONSUMEB(size);
	}

	return true;
}

//------------------------------------------------------------------------------------------------------------

void BWrepActionList::_Clear() 
{
	m_size=0;
	m_actionCount=0;

	// free actions buffer
	if(m_actions!=0) free(m_actions);
	m_actions=0;

	// free data buffer
	if(m_data!=0) free((void*)m_data);
	m_data=0;
}

//------------------------------------------------------------------------------------------------------------

BWrepActionList::~BWrepActionList() 
{
	_Clear();
}

//------------------------------------------------------------------------------------------------------------

// all actions for a same time tick
bool BWrepActionList::AddAction(BWrepAction *action)
{
	if(m_actions==0)
	{
		// first allocation
		m_size=1000;
		m_actions = (BWrepAction*)malloc(sizeof(BWrepAction)*m_size);
	}
	else if(m_actionCount>=m_size)
	{
		// grow up
		m_size+=1000;
		m_actions = (BWrepAction*)realloc(m_actions,sizeof(BWrepAction)*m_size);
	}

	// any failure?
	if(m_actions == 0) return false;

	// add action
	memcpy(&m_actions[m_actionCount],action,sizeof(BWrepAction));
	m_actionCount++;
	return true;
}

//------------------------------------------------------------------------------------------------------------

void BWrepAction::Clear() 
{
	memset(this,0,sizeof(*this));
}

//------------------------------------------------------------------------------------------------------------

const char *BWrepAction::GetName() const
{
	return BWrepGameData::GetActionNameFromID(m_ordertype);
}

//------------------------------------------------------------------------------------------------------------

// parameters as text
const char *BWrepAction::GetParameters(IUnitIDToObjectID *interf) const 
{
	char *txt = (char*)(m_pGetParamText==0?"?":m_pGetParamText(this,(const unsigned char*)m_parent->GetAbsAddress(m_dataoff),m_datasize,interf));
	char *p=strrchr(txt,'['); if(p!=0) *p=0;
	return txt;
}

// units ID as text
const char *BWrepAction::GetUnitsID(IUnitIDToObjectID *interf) const 
{
	char *txt = (char*)(m_pGetParamText==0?"?":m_pGetParamText(this,(const unsigned char*)m_parent->GetAbsAddress(m_dataoff),m_datasize,interf));
	char *p=strrchr(txt,'['); if(p==0) p=""; else p++;
	return p;
}

// pointer on parameters (must be casted to the correct BWrepActionXXX::Params)
const void *BWrepAction::GetParamStruct(int* pSize) const 
{
	if(pSize!=0) *pSize=m_datasize; return m_parent->GetAbsAddress(m_dataoff);
}

//------------------------------------------------------------------------------------------------------------

int compareAction( const void *arg1, const void *arg2 )
{
	BWrepAction *a1 = (BWrepAction *)arg1;
	BWrepAction *a2 = (BWrepAction *)arg2;
	if(a1->GetTime()>a2->GetTime()) return 1;
	if(a1->GetTime()<a2->GetTime()) return -1;
	return 0;
}

void BWrepActionList::Sort()
{
	qsort(m_actions,m_actionCount,sizeof(BWrepAction),compareAction);
}

//------------------------------------------------------------------------------------------------------------
