//----------------------------------------------------------------------------------------------------
// Replay Loader - HurtnTime+jca+fractaled (May 2003)
//----------------------------------------------------------------------------------------------------
// Classes you should use:
//
// class BWrepFile   : object to use to load a replay
// class BWrepHeader : contains replay header (replay creator, map name, all players info, etc)
// class BWrepPlayer : contains information on a player (given by BWrepHeader object)
//----------------------------------------------------------------------------------------------------
#ifndef INC_BWREPAPI_H
#define INC_BWREPAPI_H




//#include "dllexports.h"
#include "BWrepActions.h"
#include "BWrepGameData.h"
#include "BWrepMap.h"
#include <stdio.h> 
#include <time.h> 
#include <memory>
#include <string>

//void DumpRaw(const char *pszFileName, const char *outFileName);
//void DumpData(const char *pszFileName, const char *outFileName);


#pragma pack(1)

#ifndef BWLIB_NOACTIONS
// comment that line to compile without the BWRepActions.cpp file
#define USE_ACTIONS_CODE
	#ifndef BWLIB_NOMAP
		// comment that line to compile without the BWRepMap.cpp file
		#define USE_MAP_CODE
		// comment that line to compile without the RWA stuff
		#define USE_RWA_CODE
	#endif
#endif

//using namespace std;

const int kBWREP_PLAYERNAME_SIZE = 0x19;
const int kBWREP_HEADER_SIZE	 = 0x279;
const int kBWREP_ID			     = 0x53526572;
const int kBWREP_NUM_PLAYERS	 = 12;
const int kBWREP_NAME_SIZE		 = 24;
const int kBWREP_MAPNAME_SIZE	 = 26;
const int kBWREP_NUM_SLOT		 = 8;
const int kBWREP_GNAME_SIZE      = 28;

#pragma pack(1)

//
// player info
//
class BWrepPlayer
{
public:

	enum RACE
	{
		RACE_ZERG, 
		RACE_TERRAN, 
		RACE_PROTOSS, 
		RACE_RANDOM=6
	};

	enum TYPE
	{
		TYPE_NONE, 
		TYPE_COMPUTER, 
		TYPE_PLAYER
	};

	BWrepPlayer();
	~BWrepPlayer();

	//
	// Query functions
	//
	const char*	getName()		const;
	long		getNumber()		const;
	long		getSlot()		const;
	TYPE		getType()		const;
	RACE		getRace()		const;
	const char	getForce()	const;

	// Race of player
	bool isTerran()				const;
	bool isZerg()				const;
	bool isProtoss()			const;

	// Type of player
	bool isPlayer()				const;
	bool isComputer()			const;
	bool isEmpty()				const;

	//
	// Edit functions: for use by BWrepHeader
	//
	bool setName(const char* szName);
	bool setNumber(long newNumber);
	bool setSlot(long newSlot);
	bool setType(TYPE newType);
	bool setRace(RACE newRace);
	bool setForce(const char newUnknown);

private:
	long m_number;									// 0-11
	long m_slot;									// -1 if computer or none, else 0-7
	char m_type;
	char m_race;
	char m_force;									// normally 0, only for race 6 it is 1
	char m_name[kBWREP_PLAYERNAME_SIZE];
};

//
// rep header
//
class BWrepHeader
{
public:
	BWrepHeader();
	~BWrepHeader();

	enum ENGINE
	{
		ENGINE_STARCRAFT=0, 
		ENGINE_BROODWAR=1 
	};

	enum GAME_TYPE
	{
		MELEE = 2,
		FREE_FOR_ALL = 3,
		ONE_VS_ONE = 4,
		CAPTURE_THE_FLAG = 5,
		GREED = 6,
		SLAUGHTER = 7,
		SUDDEN_DEATH = 8,
		USE_MAP_SETTINGS = 10,
		TEAM_MELEE = 11,
		TEAM_FFA = 12,
		TEAM_CTF = 13,
		TOP_VS_BOTTOM = 15
	};

	//
	// Query functions
	//
	int getEngine() const {return m_engine;}
	long getGameLength() const {return m_frames;}  // in ticks
	const char*	getGameName()			const;
	const char*	getGameCreatorName()	const;
	const char* getMapName()			const;
	char		getMapType()			const;
	unsigned short getMapWidth() const {return m_mapsizeW;}
	unsigned short getMapHeight() const {return m_mapsizeH;}
	unsigned int   getCreationDate()			const {return m_creationDate;}
	BWrepHeader::GAME_TYPE	getGameType()			const;

	// get player info from the playerid in BWrepAction
	bool		getPlayerFromAction(BWrepPlayer& player, int playerid) const;

	// get player from index in player array (used for playerid in BWrepUnitDesc)
	bool		getPlayerFromIdx(BWrepPlayer& player, int idx) const;

	// Logical queries
	long		getLogicalPlayerCount()	const;
	bool		getLogicalPlayers(BWrepPlayer& player, int i) const;

	// get player color
	enum {CLR_RED,CLR_BLUE,CLR_PURPLE,CLR_ORANGE,CLR_BROWN,CLR_WHITE,CLR_YELLOW};
	int GetPlayerColor(int idx) const {return m_spotcolor[idx];}
	//
	// Edit functions
	//
	bool setGameName(const char* szName);
	bool setGameCreatorName(const char* szName);
	bool setMapType(char cMapType);
	bool setMapName(const char* szName);
	void checkPlayerNameUnicity();

private:
    char		m_engine;							// is 0x01 BW, 0x00 for SC
    long		m_frames;
    char		m_fillb;							// is 0x00 everytime
    char		m_fillc;							// is 0x00 everytime
    char		m_filld;							// is 0x48 everytime
    unsigned int      m_creationDate;				// Taiche modif for VS 2005 compilation
    char		m_ka2[8];							// every byte is 0x08
    long		m_ka3;								// is 0x00 everytime
    char		m_gamename[kBWREP_GNAME_SIZE];
	unsigned short m_mapsizeW;
	unsigned short m_mapsizeH;
    char		m_fill2[16];
    char		m_gamecreator[kBWREP_NAME_SIZE];
    char		m_maptype;							// 0x3C for bw reps
    char		m_mapname[kBWREP_MAPNAME_SIZE];
    char		m_fill3[6];
	char		m_gameType;
	char		m_fill4[31];
    BWrepPlayer m_oPlayer[kBWREP_NUM_PLAYERS];
    long		m_spotcolor[8];						// player color (thx to TravelToAiur)
    char		m_spot[8];							// unknown, flag if spot[n] is used
};

//----------------------------------------------------------------------------------------------------

//
// user rep file access
//
class BWrepFile
{
public:
	BWrepFile();
	virtual ~BWrepFile();

	// load replay (at least the header)
	enum {LOADMAP=1, LOADACTIONS=2, ADDACTIONS=4};
	int Load(const char* pszFileName, int options=LOADMAP|LOADACTIONS, void *rwaheader=0, int size=0);

	// get offset in file of audio part (if any, 0 if none)
	unsigned long GetAudioOffset(const char* pszFileName, void *header, int size);

private:
	bool _Open(const char* pszFileName);
	bool _Close();
	bool _IsRepFile(); // added by Taiche : checks if the header looks like a replay's
	bool _LoadActions(FILE *fp, bool clear, bool decode);
	bool _LoadMap(FILE *fp, bool decode);

	FILE*		m_pFile;

    //std::auto_ptr<BWrepRWTData> rwtData; //added chrisbk 14 02 2006

public:

    //const BWrepRWTData * getRWTData() const { return rwtData.get();};

	// replay header
	BWrepHeader m_oHeader;


	// actions list
	#ifdef USE_ACTIONS_CODE
	BWrepActionList m_oActions;
	#endif

	// game map
	#ifdef USE_MAP_CODE
	BWrepMap m_oMap;
	#endif

	// offset of RWA (if any)
	#ifdef USE_RWA_CODE
	unsigned long m_rwaoffset;
	#endif
};


#pragma pack() //restore alignement
#endif // INC_BWREPAPI_H
