#include "stdafx.h"
#include "BWlib/BWrepAPI.h"
#include "BWlib/BWrepGameData.h"
#include "unpack.h"
#include <string.h>
#include <errno.h>
//#include <fstream>

int unpack_section(FILE *file, byte *result, int size);

//
// BWrepPlayer: player information
//
BWrepPlayer::BWrepPlayer()
{
	m_number  = 0;
	m_slot    = 0;
	m_type    = TYPE_NONE;
	m_race    = RACE_ZERG;	// TODO: is this an appropriate default?
	m_force = 0;
	strcpy(m_name, "");
}

BWrepPlayer::~BWrepPlayer()
{
}

bool BWrepPlayer::isTerran() const
{
	return (m_race == RACE_TERRAN) ? true : false;
}

bool BWrepPlayer::isZerg() const
{
	return (m_race == RACE_ZERG) ? true : false;
}

bool BWrepPlayer::isProtoss() const
{
	return (m_race == RACE_PROTOSS) ? true : false;
}

bool BWrepPlayer::isPlayer() const
{
	return (m_type == TYPE_PLAYER) ? true : false;
}

bool BWrepPlayer::isComputer() const
{
	return (m_type == TYPE_COMPUTER) ? true : false;
}

bool BWrepPlayer::isEmpty() const
{
	return (m_type == TYPE_NONE) ? true : false;
}

const char* BWrepPlayer::getName() const
{
	return m_name;
}

long BWrepPlayer::getNumber() const
{
	return m_number;
}

long BWrepPlayer::getSlot() const
{
	return m_slot;
}

BWrepPlayer::TYPE BWrepPlayer::getType() const
{
	return static_cast<TYPE>(m_type);
}

BWrepPlayer::RACE BWrepPlayer::getRace() const
{
	return static_cast<RACE>(m_race);
}

const char BWrepPlayer::getForce() const
{
	return m_force;
}

bool BWrepPlayer::setName(const char* szName)
{
	strcpy(m_name, szName);
	return true;
}

bool BWrepPlayer::setNumber(long newNumber)
{
	if (newNumber >= 0 && newNumber <= 11)
	{
		m_number = newNumber;
	}
	else
	{
		return false;
	}
	return true;
}

bool BWrepPlayer::setSlot(long newSlot)
{
	if (newSlot >= -1 && newSlot < kBWREP_NUM_SLOT)
	{
		m_slot = newSlot;
	}
	else
	{
		return false;
	}
	return true;
}

bool BWrepPlayer::setType(TYPE newType)
{
	if (newType >= TYPE_NONE && newType <= TYPE_PLAYER)
	{
		m_type = newType;
	}
	else
	{
		return false;
	}
	return true;
}

bool BWrepPlayer::setRace(RACE newRace)
{
	if (newRace >= RACE_ZERG && newRace <= RACE_RANDOM)
	{
		m_race = newRace;
	}
	else
	{
		return false;
	}
	return true;
}

bool BWrepPlayer::setForce(const char newForce)
{
	m_force = newForce;
	/*if (newForce == 0 || newForce == 1)
	{
		m_force = newForce;
	}
	else
	{
		return false;
	}*/
	return true;
}

//
// BWrepHeader: replay file header
//
BWrepHeader::BWrepHeader()
{
}

BWrepHeader::~BWrepHeader()
{
}

const char* BWrepHeader::getGameName() const
{
	return m_gamename;
}

const char* BWrepHeader::getGameCreatorName() const
{
	return m_gamecreator;
}

const char* BWrepHeader::getMapName() const
{
	return m_mapname;
}

char BWrepHeader::getMapType() const
{
	return m_maptype;
}

BWrepHeader::GAME_TYPE BWrepHeader::getGameType() const
{
	return (BWrepHeader::GAME_TYPE)m_gameType;
}

// get player from index in player array (used for playerid in BWrepUnitDesc)
bool BWrepHeader::getPlayerFromIdx(BWrepPlayer& player, int idx) const 
{
	if (idx >= 0 && idx < kBWREP_NUM_PLAYERS) 
	{
		player=m_oPlayer[idx];
		return true;
	}
	return false;
}

bool BWrepHeader::getPlayerFromAction(BWrepPlayer& player, int playerid) const
{
	if (playerid >= 0 && playerid < kBWREP_NUM_PLAYERS)
	{
		for (long i = 0; i < kBWREP_NUM_PLAYERS; ++i)
		{
			if (m_oPlayer[i].getSlot()==playerid)
			{
				player = m_oPlayer[i];
				return true;
			}
		}
	}
	return false;
}

void BWrepHeader::checkPlayerNameUnicity()
{
	bool duplicate;
	do
	{
		duplicate=false;
		for (int i = 1; i < kBWREP_NUM_PLAYERS; i++)
		{
			if(m_oPlayer[i].getName()[0]==0) continue;
			for (int j = 0; !duplicate && j < kBWREP_NUM_PLAYERS; j++)
			{
				if (i!=j && strcmp(m_oPlayer[i].getName(),m_oPlayer[j].getName())==0)
				{
					// we have a duplicate, change name
					char newname[kBWREP_PLAYERNAME_SIZE];
					strcpy(newname,m_oPlayer[j].getName());
					strcat(newname,".2");
					m_oPlayer[j].setName(newname);
					duplicate=true;
					break;
				}
			}
		}
	}
	while(duplicate);
}

bool BWrepHeader::setGameName(const char* szName)
{
	strcpy(m_gamename, szName);
	return true;
}

bool BWrepHeader::setGameCreatorName(const char* szName)
{
	strcpy(m_gamecreator, szName);
	return true;
}

bool BWrepHeader::setMapType(char cMapType)
{
	m_maptype = cMapType;
	return true;
}

bool BWrepHeader::setMapName(const char* szName)
{
	strcpy(m_mapname, szName);
	return true;
}

long BWrepHeader::getLogicalPlayerCount() const
{
	long nPlayerCount = 0;
	for (long i = 0; i < kBWREP_NUM_PLAYERS; ++i)
	{
		if (!m_oPlayer[i].isEmpty())
		{
			++nPlayerCount;
		}
	}
	return nPlayerCount;
}

bool BWrepHeader::getLogicalPlayers(BWrepPlayer& player, int idxPlayer) const
{
	for (long i = 0,j=0; i < kBWREP_NUM_PLAYERS; ++i)
	{
		if (!m_oPlayer[i].isEmpty())
		{
			if(j==idxPlayer) {player=m_oPlayer[i]; return true;}
			j++;
		}
	}
	return false;
}

//
// BWrepFile: user rep file access
//
BWrepFile::BWrepFile()
{
	m_pFile	= NULL;
}

BWrepFile::~BWrepFile()
{
	_Close();
}

bool BWrepFile::_Open(const char* pszFileName)
{
	if(pszFileName[0]==0) return false;
    m_pFile = fopen(pszFileName, "rb");
	return (m_pFile != NULL) ? true : false;
}

bool BWrepFile::_Close()
{
	if (m_pFile != NULL)
	{
		fclose(m_pFile);
		m_pFile=0;
	}
	return true;
}

bool BWrepFile::_IsRepFile()
{
	if(NULL != m_pFile)
	{
		fseek(m_pFile, 0, SEEK_END);
		long fileSize = ftell(m_pFile);
		rewind(m_pFile);

		if(fileSize > 16)
		{
			unsigned char header[16];
			fread(header, 1, 16, m_pFile);
			rewind(m_pFile);
			if(header[0] == 0xA7 && header[1] == 0x7E && header[2] == 0x7E && header[3] == 0x2B
				&& header[4] == 0x01 && header[5] == 0x00 && header[6] == 0x00 && header[7] == 0x00
				&& header[8] == 0x04 && header[9] == 0x00 && header[10] == 0x00 && header[11] == 0x00
				&& header[12] == 'r' && header[13] == 'e' && header[14] == 'R' && header[15] == 'S')

				return true;
		}
	}

	return false;
}

/*void DumpRaw(const char *pszFileName, const char *outFileName)
{
	FILE *repFile = fopen(pszFileName, "rb");
	if(repFile != NULL)
	{
		// repID (reRS)
		byte *nRepID = (byte *)malloc(4);
		unpack_section(repFile, (byte*)nRepID, 4);

		// Header
		byte *header = (byte *)malloc(kBWREP_HEADER_SIZE);
		unpack_section(repFile, header, kBWREP_HEADER_SIZE);
		//header.checkPlayerNameUnicity();


		// Actions
		int cmdSize = 0;
		unpack_section(repFile, (byte*)&cmdSize, sizeof(cmdSize));

		byte *buffer = (byte *)malloc(cmdSize);
		unpack_section(repFile, buffer, cmdSize);


		// Map
		int mapSize = 0;
		unpack_section(repFile, (byte*)&mapSize, sizeof(mapSize));

		byte *bufferMap = (byte *)calloc(mapSize,sizeof(byte));
		unpack_section(repFile, bufferMap, mapSize);

		// Let's write all this to output
		std::ofstream out(outFileName, std::ios::out | std::ios::binary);
		out.write((const char *)nRepID, 4);
		out.write((const char *)header, kBWREP_HEADER_SIZE);
		out.write((const char *)buffer, cmdSize);
		out.write((const char *)bufferMap, mapSize);
		out.close();

		free(header);
		free(nRepID);

		fclose(repFile);
	}
}

void DumpData(const char *pszFileName, const char *outFileName)
{
	FILE *repFile = fopen(pszFileName, "rb");
	if(repFile != NULL)
	{
		// repID (reRS)
		long nRepID;
		unpack_section(repFile, (byte*)&nRepID, sizeof(nRepID));

		// Header
		BWrepHeader header;
		unpack_section(repFile, (byte*)&header, kBWREP_HEADER_SIZE);
		header.checkPlayerNameUnicity();


		// Actions
		int cmdSize = 0;
		unpack_section(repFile, (byte*)&cmdSize, sizeof(cmdSize));

		byte *buffer = (byte *)malloc(cmdSize);
		// unpack cmd section in buffer
		unpack_section(repFile, buffer, cmdSize);

		BWrepActionList actions;
		// decode all actions (dont free buffer, it belongs to actions)
		if(!actions.DecodeActions(buffer, cmdSize, true))
		{
			free(buffer);
			buffer = NULL;
		}


		// Map : same thing
		int mapSize = 0;
		unpack_section(repFile, (byte*)&mapSize, sizeof(mapSize));

		byte *bufferMap = (byte *)calloc(mapSize,sizeof(byte));
		unpack_section(repFile, bufferMap, mapSize);

		// decode map (dont free buffer, it belongs to m_oMap now)
		BWrepMap map;
		map.DecodeMap(bufferMap, mapSize, header.getMapWidth(), header.getMapHeight());


		// Let's write all this to output
		std::ofstream out(outFileName, std::ios::out | std::ios::binary);
		out << nRepID << std::endl;
		out << header.getEngine() << std::endl;
		out << header.getGameLength() << std::endl;
		out << header.getCreationDate() << std::endl;
		out << header.getMapWidth() << std::endl;
		out << header.getMapHeight() << std::endl;
		out << header.getGameCreatorName() << std::endl;
		out << header.getMapType() << std::endl;
		out << header.getMapName() << std::endl;
		out << header.getGameType() << std::endl;
		for(int i = 0; i < 12; i++)
		{
			BWrepPlayer curPlayer;
			header.getPlayerFromIdx(curPlayer, i);
			if(!curPlayer.isEmpty())
			{
				out << curPlayer.getNumber() << std::endl;
				out << curPlayer.getSlot() << std::endl;
				out << curPlayer.getType() << std::endl;
				out << curPlayer.getRace() << std::endl;
				out << curPlayer.getForce() << std::endl;
				out << curPlayer.getName() << std::endl;
			}
		}
		out.close();

		fclose(repFile);
	}
}*/
  
//------------------------------------------------------------------------------------------------------------

int BWrepFile::Load(const char* pszFileName, int options, void *rwaheader, int size)
{

	int rc = 0;

	// offset of RWA (if any)
#ifdef USE_RWA_CODE
	m_rwaoffset=0;
#endif

	bool bOk = _Open(pszFileName);
	if(bOk)
	{
		bOk = _IsRepFile();
		if(bOk)
		{
			// Bon alors la c'est juste un check pour voir qu'on a bien reRS dans le replay [:petrus75]
			long nRepID;
			unpack_section(m_pFile, (byte*)&nRepID, sizeof(nRepID));

			bOk = (nRepID == kBWREP_ID);
			if(bOk)
			{
				// read header
				bOk = (unpack_section(m_pFile, (byte*)&m_oHeader, kBWREP_HEADER_SIZE)==0);
				m_oHeader.checkPlayerNameUnicity();

				// read actions
#ifdef USE_ACTIONS_CODE
				if(bOk)
					bOk = _LoadActions(m_pFile,(options&ADDACTIONS)==0,(options&LOADACTIONS)!=0);
				else
					rc = -3;

				// load map
#ifdef USE_MAP_CODE
				if(bOk) 
				{
					bOk = _LoadMap(m_pFile, (options&LOADMAP)!=0);

#ifdef USE_RWA_CODE
					if(bOk && rwaheader!=0)
					{
						// read audio header and return file pointer on audio data
						memset(rwaheader,0,size);
						unsigned long *headerSize; headerSize=(unsigned long *)rwaheader;
						if(fread(rwaheader,1,sizeof(long),m_pFile)==sizeof(long))
						{
							size_t readsize; readsize = min((unsigned long)size,*headerSize)-sizeof(long);
							fread(((char*)rwaheader)+sizeof(long),1,readsize,m_pFile);
							if(*headerSize>(unsigned long)size) fseek(m_pFile,(*headerSize)-size,SEEK_CUR);
							m_rwaoffset = ftell(m_pFile);
						}
					}
	#endif
				}
				else
					rc = -4;

                /*if (bOk) //Added by Chrisbk 14/02/2006 : parse RWT info
                {
                    rwtData.reset(new BWrepRWTData(m_pFile));
                }*/
#endif
#endif
			}
			else
				rc = -2;
		}
		else
			rc = -5;

		_Close();
	}
	else
		return errno;

	/*if (!bOk)
		return -1;*/
	return rc;
}

//------------------------------------------------------------------------------------------------------------

// get offset in file of audio part (if any, 0 if none)
unsigned long BWrepFile::GetAudioOffset(const char* pszFileName, void *header, int size)
{
	unsigned long offset=0;

	bool bOk = _Open(pszFileName);
	if(!bOk) return 0;

	long nRepID;
	int res = unpack_section(m_pFile, (byte*)&nRepID, sizeof(nRepID));
	if(nRepID != kBWREP_ID) goto Exit;
	if(res!=0) goto Exit;
						
	// read header
	bOk = (unpack_section(m_pFile, (byte*)&m_oHeader, kBWREP_HEADER_SIZE)==0);
	if(!bOk) goto Exit;

	// get actions section size
	int cmdSize; cmdSize=0;
	res = unpack_section(m_pFile, (byte*)&cmdSize, sizeof(cmdSize));
	if(res!=0) goto Exit;

	// alloc buffer to read it
	byte *buffer; buffer = (byte *)malloc(cmdSize * sizeof(byte));
	if (buffer==0) goto Exit;

	// unpack cmd section in buffer
	res = unpack_section(m_pFile, buffer, cmdSize);
	if(res!=0) goto Exit;
	free(buffer);

	// get map section size
	int mapSize; mapSize=0;
	res = unpack_section(m_pFile, (byte*)&mapSize, sizeof(mapSize));
	if(res!=0) goto Exit;

	// alloc buffer to read it
	buffer = (byte *)malloc(mapSize * sizeof(byte));
	if (buffer==0) goto Exit;

	// unpack map section in buffer
	res = unpack_section(m_pFile, buffer, mapSize);
	if(res!=0) goto Exit;
	free(buffer);

	// read audio header and return file pointer on audio data
	unsigned long *headerSize; headerSize=(unsigned long *)header;
	if(fread(header,1,sizeof(long),m_pFile)!=sizeof(long)) goto Exit;
	size_t readsize; readsize = min((unsigned long)size,*headerSize)-sizeof(long);
	fread(((char*)header)+sizeof(long),1,readsize,m_pFile);
	if(*headerSize>(unsigned long)size) fseek(m_pFile,(*headerSize)-size,SEEK_CUR);
	offset = ftell(m_pFile);

Exit:
	_Close();
	return offset;
}

//------------------------------------------------------------------------------------------------------------

// must be called after Load
#ifdef USE_ACTIONS_CODE
bool BWrepFile::_LoadActions(FILE *fp, bool clear, bool decode)
{
	// get section size
	int cmdSize=0;
	unpack_section(fp, (byte*)&cmdSize, sizeof(cmdSize));

	// alloc buffer to read it
	byte *buffer = (byte *)malloc(cmdSize * sizeof(byte));
	if (buffer==0) return false;

	// unpack cmd section in buffer
	unpack_section(fp, buffer, cmdSize);

	// decode all actions (dont free buffer, it belongs to m_oActions now)
	bool bOk = decode ? m_oActions.DecodeActions(buffer,cmdSize, clear) : true;
	if(!decode) free(buffer);

	return bOk;
}
#endif

//------------------------------------------------------------------------------------------------------------
	  
// must be called after LoadActions
#ifdef USE_MAP_CODE
bool BWrepFile::_LoadMap(FILE *fp, bool decode)
{
	// get section size
	int mapSize=0;
	unpack_section(fp, (byte*)&mapSize, sizeof(mapSize));

	// alloc buffer to read it
	byte *buffer = (byte *)calloc(mapSize,sizeof(byte));
	if (buffer==0) return false;

	// unpack map section in buffer
	unpack_section(fp, buffer, mapSize);

	// decode map (dont free buffer, it belongs to m_oMap now)
	bool bOk = decode?m_oMap.DecodeMap(buffer,mapSize,m_oHeader.getMapWidth(),m_oHeader.getMapHeight()):true;
	if(!decode) free(buffer);

	return bOk;
}
#endif

//------------------------------------------------------------------------------------------------------------
