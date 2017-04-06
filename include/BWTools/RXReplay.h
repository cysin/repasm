#ifndef __RX____REPLAY
#define __RX____REPLAY

#include <string>
#include <vector>
#include <iostream>
#include <stdexcept>
#include "RXTeam.h"
#include <stdexcept>

class BWrepFile;
class BWrepPlayer;

class RXPlayer;
class RXPlayerImpl;
class RXVersion;
class RXVersionFromDate;
class RXMapInfo;


class RXInvalidReplayException : public std::runtime_error
{
public:
	RXInvalidReplayException(const char *message);
};


class RXReplay
{
public:

	enum GameType
	{
		MELEE,
		FREE_FOR_ALL,
		ONE_VS_ONE,
		CAPTURE_THE_FLAG,
		GREED,
		SLAUGHTER,
		SUDDEN_DEATH,
		USE_MAP_SETTINGS,
		TEAM_MELEE,
		TEAM_FFA,
		TEAM_CTF,
		TOP_VS_BOTTOM,
		UNKNOWN
	};


	RXReplay(const BWrepFile &from);
	RXReplay(std::istream &stream);

	~RXReplay(void);

	GameType getGameType() const { return gameType;}

	const RXPlayerList & getPlayers() const { return players;}
	const RXTeamList   & getTeams()   const { return teamList;}

	const std::string & getGameName()			const {return gameName;}
	//const std::string & getMapName()			const {return mapName;}
	const std::string & getGameTypeStr()		const {return gameTypeStr;}
	const std::string & getFormattedLength()	const {return formGameLength;};
	const std::string & getFormattedDate()		const {return formGameDate;};
	const std::string & getWinnerName()		const {return winnerName;};

	const std::string &getMatchup() const {return matchup;}

	bool IsBroodWar() const  {return bBroodWar;}

	int    getGameLength() const  {return gameLength;}
	unsigned int getGameDate()   const  {return gameDate;}
	unsigned int getWinnerProbability()   const  {return winProbability;}

	const RXVersion * getVersion() const {return (RXVersion *)version;}
	const RXMapInfo * getMapInfo() const {return mapInfo;}
	//const std::vector<int> & getStartLocs()		const {return mapInfo->;}

	void serialize(std::ostream &str) const;
	void setVersionPath(const std::string& filePath);

private:

	void checkReplay(const BWrepFile &from);
	GameType gameType;

	std::string formGameLength;
	std::string formGameDate;
	std::string gameName;
	std::string matchup;
	std::string gameTypeStr;
	std::string winnerName;
	std::string versionFilePath;
 
	RXPlayerList players;
	RXTeamList teamList;

	RXVersionFromDate *	version;
	RXMapInfo * mapInfo;
	//std::vector<int> startLocs;

	bool   bBroodWar;
	int    gameLength;
	unsigned int gameDate;
	unsigned short winProbability;

	void		   extractPlayers(const BWrepFile &file);
	RXPlayerImpl * extractPlayer(const BWrepFile &file, const BWrepPlayer &player);

	void buildFormatedInfo();
	void guessVersionNumber();
	void createGameType();
	void buildMapInfo(const BWrepFile &file);
	void findWinner(const BWrepFile &from);
};

#endif
