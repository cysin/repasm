#include "stdafx.h"
#include "BWTools/RXReplay.h"
#include <BWlib/BWrepAPI.h>
#include <sstream>
#include <iomanip>
#include <time.h>

#include "RXPlayerImpl.h"
#include "RXPlayerImport.h"
#include "RXTeamManager.h"
#include "RXWinner.h"
#include "RXMatchup.h"

#include "RXAPM.h"
#include "rxmapbase/RXStartingLocation.h"
#include "RXIsObs.h"

#include "RXVersionFromDate.h"
#include "BWTools/RXMapInfo.h"

#include <rxmapbase/RXRepMap.h>


// When adding/removing/modifying serialized data, change the version here. This will force the repcache files to update (instead of... crashing explorer.exe :p)
static const char * gSerializationVersion = "RXR07";


RXInvalidReplayException::RXInvalidReplayException(const char *message)
: std::runtime_error(std::string("Invalid replay : ") + message)
{
}


RXReplay::RXReplay(const BWrepFile &file)
{
	checkReplay(file);

	version = NULL;

	gameName = file.m_oHeader.getGameName();

	if (file.m_oHeader.getEngine())
		bBroodWar = true;
	else
		bBroodWar = false;

	gameLength = BWrepAction::Tick2Sec(file.m_oHeader.getGameLength());
	gameDate   = file.m_oHeader.getCreationDate();
	winProbability = 0;

	// Game type as a string

	switch (file.m_oHeader.getGameType())
	{
	case BWrepHeader::MELEE:
		gameType = MELEE;
		break;
	case BWrepHeader::FREE_FOR_ALL:
		gameType = FREE_FOR_ALL;
		break;
	case BWrepHeader::ONE_VS_ONE:
		gameType = ONE_VS_ONE;
		break;
	case BWrepHeader::CAPTURE_THE_FLAG:
		gameType = CAPTURE_THE_FLAG;
		break;
	case BWrepHeader::GREED:
		gameType = GREED;
		break;
	case BWrepHeader::SLAUGHTER:
		gameType = SLAUGHTER;
		break;
	case BWrepHeader::SUDDEN_DEATH:
		gameType = SUDDEN_DEATH;
		break;
	case BWrepHeader::USE_MAP_SETTINGS:
		gameType = USE_MAP_SETTINGS;
		break;
	case BWrepHeader::TEAM_MELEE:
		gameType = MELEE;
		break;
	case BWrepHeader::TEAM_FFA:
		gameType = TEAM_FFA;
		break;
	case BWrepHeader::TEAM_CTF:
		gameType = TEAM_CTF;
		break;
	case BWrepHeader::TOP_VS_BOTTOM:
		gameType = TOP_VS_BOTTOM;
		break;
	default:
		gameType = UNKNOWN;
		break;
	}

    RXRepMap map(file);
	mapInfo = new RXMapInfo(&map);

	extractPlayers(file);
	buildFormatedInfo();
	findWinner(file); // Must be called AFTER players extraction
}

RXReplay::~RXReplay(void)
{
	//clean up player List
	for (size_t i=0;i<players.size();i++)
		delete players[i];

	if (version)
		delete version;

	if(mapInfo)
		delete mapInfo;
}




RXReplay::RXReplay(std::istream &str)
{
    std::string header;
    std::getline(str, header, '\0');

    if (header != gSerializationVersion)
        throw RXInvalidReplayException("Not a valid replay file");


	std::getline(str, gameName, '\0');
	std::getline(str, matchup, '\0');

	str.read((char *)&gameType,sizeof(gameType));

	str.read((char *)&bBroodWar,sizeof(bBroodWar));
	str.read((char *)&gameLength,sizeof(gameLength));
	str.read((char *)&gameDate,sizeof(gameDate));

	int numPlayer;
	str.read((char *)&numPlayer,sizeof(numPlayer));

	for (int i=0;i<numPlayer;i++)
		players.push_back(new RXPlayerImport(str));

	teamList = RXTeamManager::import(str, players);
	mapInfo = new RXMapInfo(str);

	// Winner (added in RXR07)
	std::getline(str, winnerName, '\0');
	str.read((char *)&winProbability,sizeof(winProbability));


	buildFormatedInfo();
}


void RXReplay::serialize(std::ostream &str) const 
{
    str << gSerializationVersion<<'\0';

	str << gameName <<'\0';
	str << matchup << '\0';

	str.write((const char *)&gameType,sizeof(gameType));
	str.write((const char *)&bBroodWar,sizeof(bBroodWar));
	str.write((const char *)&gameLength,sizeof(gameLength));
	str.write((const char *)&gameDate,sizeof(gameDate));

	int numPlayer = (int)players.size();
	str.write((const char *)&numPlayer,sizeof(numPlayer));

	for (int i=0;i<numPlayer;i++)
		players[i]->serialize(str);

	RXTeamManager::serialize(str, teamList);
	if(mapInfo != NULL)
		mapInfo->serialize(str);

	// Winner (added in RXR07)
	str << winnerName << '\0';
	str.write((const char *)&winProbability,sizeof(winProbability));
}





void RXReplay::extractPlayers(const BWrepFile &file)
{
	int numPlayer = file.m_oHeader.getLogicalPlayerCount();

	players.reserve(numPlayer);

	for (int i=0;i<numPlayer;i++) {
		BWrepPlayer pl;
		if (file.m_oHeader.getLogicalPlayers(pl, i)) {
			RXPlayerImpl *impl = extractPlayer(file, pl);
			players.push_back(impl);
		}
	}

	RXTeamManager manager(file, players);
	this->teamList = manager.getTeamList();
	if(this->teamList.size() > 0)
		this->matchup = RXMatchup::getMatchup(this->teamList);
	else
		this->matchup = RXMatchup::getMatchup(players);
}




RXPlayerImpl * RXReplay::extractPlayer(const BWrepFile &file, const BWrepPlayer &player)
{
	RXPlayerImpl *impl = new RXPlayerImpl(file, player);


	impl->setIsObs(RXIsObs::IsObs(file, player));
	impl->setAPM(RXAPM::compute(file, player));

	RX2dCoord coord;
	int s4,s12;
	s12 = RXStartingLocation::getStartingPosition12(file,player,coord);
	s4  = RXStartingLocation::getStartingPosition4(file,player,coord);

	impl->setStartingPos(s4,s12,coord);

	return impl;
}

void RXReplay::createGameType()
{
	switch (gameType)
	{
	case MELEE:
		gameTypeStr = "Melee";
		break;
	case FREE_FOR_ALL:
		gameTypeStr = "Free For All";
		break;
	case ONE_VS_ONE:
		gameTypeStr = "One vs One";
		break;
	case CAPTURE_THE_FLAG:
		gameTypeStr = "Capture The Flag";
		break;
	case GREED:
		gameTypeStr = "Greed";
		break;
	case SLAUGHTER:
		gameTypeStr = "Slaughter";
		break;
	case SUDDEN_DEATH:
		gameTypeStr = "Sudden Death";
		break;
	case USE_MAP_SETTINGS:
		gameTypeStr = "Use Map Settings";
		break;
	case TEAM_MELEE:
		gameTypeStr = "Team Melee";
		break;
	case TEAM_FFA:
		gameTypeStr = "Team FFA";
		break;
	case TEAM_CTF:
		gameTypeStr = "Team CTF";
		break;
	case TOP_VS_BOTTOM:
		gameTypeStr = "Top vs Bottom";
		break;

	case UNKNOWN:
	default:
		gameTypeStr = "Unknown";
		break;
	}
}
							 

void RXReplay::buildFormatedInfo()
{
	std::ostringstream oss(std::ostringstream::out);
	oss << std::setfill('0') << std::setw(2);
	
	int nHrs = (int)gameLength / 3600;
	int nMin = (int)(gameLength - nHrs * 3600) / 60;
	int nSec = (int)(gameLength - nHrs * 3600 - nMin * 60);

	oss << nHrs << ":" << std::setw(2) << nMin << ":" << std::setw(2) << nSec;
	formGameLength = oss.str().c_str();

	//Format the date
	char dest[256];

	time_t youpi = (time_t) gameDate;
	if (localtime(&youpi))
		strftime(dest,256,"%Y/%m/%d %H:%M",localtime(&youpi));
	else
		sprintf(dest, "Invalid game date");

	formGameDate = dest;

	guessVersionNumber();
	createGameType();
}


void RXReplay::guessVersionNumber()
{
	if(versionFilePath != "")
		version = new RXVersionFromDate(versionFilePath);
	else
		version = new RXVersionFromDate();
	version->buildFromDate(gameDate);
}

void RXReplay::setVersionPath(const std::string &filePath)
{
	versionFilePath = filePath;
	guessVersionNumber();
}


void RXReplay::checkReplay(const BWrepFile &from)
{
	if (from.m_oMap.GetSection(SECTION_UNIT) == NULL)
		throw RXInvalidReplayException("No UNIT section");

}

void RXReplay::findWinner(const BWrepFile &from)
{
	if(players.size() == 0)
		return;

	RXWinner winFinder(from, players);
	int winnerNum = -1;
	winFinder.getWinner(winnerNum, winProbability);

	if(winnerNum > -1)
		winnerName = players[winnerNum]->getName().c_str();
}
