//Small PHP Extension that uses BWChart to extract a couple of information from a .rep file
//
// Notes :
//  - This was written using VS 8.0 (aka Visual Studio 2005) and was tested with gcc/g++ 4.1.2.
//
//  - The PHP API seems to be quite a mess and the large amount of warnings ('argument' : conversion from 'size_t' to 'uint', possible loss of data)
//    are his, not mine.
//
//  - We have no 64-bit computer/OS to test on ; from what I've heard, it's likely StormLib will fail to compile on
//    64-bit Unixes because of cast errors. Just cast the int to long and it should work better. However, these
//    platforms and modifications are not supported yet.
//
// Use it freely, modify it, whatever you like, but keep in mind that this comes with absolutely not warranty
//

// PHP seems to mess things up with VS 2005 and winsock ; this define is some by-pass to make RepASM build
// If you have a clean fix, I'd like to know it so feel free to contact us !
#define WIN32_LEAN_AND_MEAN

#define REPASM_VERSION "4.1.3"

// This is for linker problems with VS : these includes must be placed BEFORE the extern "C" directive...
// WARNING : DO NOT TRY TO INCLUDE ANYTHING USING RXFILE IN THERE ! Do it in repasmmap instead. This is a really weird
// behaviour from gcc 4.1.2 that I have been unable to solve. Again, any info welcome.
#include "BWlib/BWrepAPI.h"
#include "BWTools/RXReplay.h"
#include "BWTools/RXPlayer.h"
#include "BWTools/RXColor.h"
#include "BWTools/RXVersion.h"
#include "BWTools/RXMapInfo.h"
#include "BWTools/RXActionCompiler.h"
#include "repasmmap.h"
#include <rxmaptools/RXMinimap.h>
#include <rxmapbase/RXCHKMap.h>
#include <rxmapbase/RXMapExtractor.h>
#include <stdio.h>
#include <string.h>

extern "C" {
#include "php.h"
#include "ext/standard/info.h"
}




/*
********************************
*			TOOLS
********************************
*/


//PHP API doesn't seem to know about constness....
static void addString(zval *val, char * key, const char *value) 
{
	TSRMLS_FETCH();
	char * stupidAPI = strdup(value);
	add_property_string(val, key, stupidAPI,1u);
	free(stupidAPI);
}

// Returns 0 if the file could not be opened, 1 if it is a FPREP, 2 if RWT only, 3 if normal replay.
static int getRWTStatus(const char *repPath)
{
	FILE *pReplay = fopen(repPath, "rb");
	if(pReplay != NULL)
	{
		// States if it's a RWT/FPREP
		char sig[4] = {0,0,0,0};
		fseek(pReplay, -11, SEEK_END);
		fread(sig, 1, 3, pReplay);

		if (strcmp(sig, "TWR") == 0)
		{
			int rwtSize = 0;
			// Gets the offset where the RWT data end
			fseek(pReplay, -16, SEEK_END);
			int endRWT = (int)ftell(pReplay) - 16;
			// Reads the RWT size
			fread((char *)&rwtSize, 1, 4, pReplay);
			// Skips the intro (Replay saved by blahblah)
			fseek(pReplay, -rwtSize, SEEK_CUR);
			char curChar = ' ';
			while(curChar != '\0')
				curChar = fgetc(pReplay);

			fseek(pReplay, 4, SEEK_CUR);
			curChar = fgetc(pReplay);
			fclose(pReplay);

			// It's an FPREP
			if(curChar == '^')
				return 1;

			// It's only RWT
			return 2;
		}
		fclose(pReplay);

		return 3; // Not RWT
	}


	return 0; // Could not open
}


/*
********************************
*			GETTERS
********************************
*/


//Add replay global info  : Number of player, map and length of the game (in s)
static void addGlobalInfo(const RXReplay &replay, zval *return_val) 
{
	TSRMLS_FETCH();
	zval *winnerObject;
	MAKE_STD_ZVAL(winnerObject);
	object_init(winnerObject);

	add_property_long(return_val,   "NumPlayer",  replay.getPlayers().size());
	add_property_long(return_val,   "GameLength", replay.getGameLength());
	add_property_long(return_val,   "GameDate", replay.getGameDate());

	addString(return_val, "GameName",  replay.getGameName().c_str());
	addString(return_val, "GameType", replay.getGameTypeStr().c_str());
	add_property_long(return_val, "IsBroodWar", replay.IsBroodWar());

	if(replay.getWinnerName() != "")
	{
		addString        (winnerObject, "Name", replay.getWinnerName().c_str());
		add_property_long(winnerObject, "Probability", (long)replay.getWinnerProbability());
		add_property_zval(return_val, "Winner", winnerObject);
	}
}



//=====================================================
// Players info
//=====================================================

static zval * createOnePlayerObject(const RXPlayer *player) {

	//Create a new php object
	TSRMLS_FETCH();
	zval *playerObject;
	MAKE_STD_ZVAL(playerObject);
	object_init(playerObject);

	addString        (playerObject,"Name",player->getName().c_str());
	add_property_long(playerObject,"Race",player->getRace()->getRaceID());
	addString        (playerObject,"RaceName",player->getRace()->getName().c_str());

	add_property_long(playerObject,"Number"     ,(uint)player->getPlayerID());
	add_property_long(playerObject,"APM"        ,(uint)player->getAPM());
	add_property_bool(playerObject,"Human"      ,player->isHuman());
	add_property_bool(playerObject,"Computer"   ,player->isComputer());
	add_property_long(playerObject,"Color"      ,player->getColor()->getColorID());
	addString        (playerObject,"ColorName"  ,player->getColor()->getName().c_str());
	add_property_long(playerObject,"ColorHTML"  ,player->getColor()->getHTMLCode());
	add_property_bool(playerObject,"IsObserver" ,player->isObs());


	add_property_long(playerObject,"StartingLocation4"      ,player->getStartPos4());
	add_property_long(playerObject,"StartingLocation12"     ,player->getStartPos12());

	add_property_double(playerObject,"NormXpos"      ,player->getNormalizedPos().x);
	add_property_double(playerObject,"NormYpos"      ,player->getNormalizedPos().y);


    //disconnec info
    add_property_bool(playerObject,"DidLeave"   ,player->didLeave());
    add_property_long(playerObject,"LeaveStamp"      ,player->getLeaveStamp());


	return playerObject;

}


static void addPlayerInfo(const RXReplay &replay, zval *return_val) 
{
	zval *playerArray;
	TSRMLS_FETCH();
	MAKE_STD_ZVAL(playerArray);

	array_init(playerArray);

	const RXPlayerList &list = replay.getPlayers();

	for (size_t i=0;i<list.size();i++)
		add_next_index_zval(playerArray, createOnePlayerObject(list[i]));

	add_property_zval(return_val, "Players",playerArray);
}



//=====================================================
// Team info
//=====================================================

static void addTeamInfo(const RXReplay &rep, zval *return_val) 
{
	const RXTeamList &teams = rep.getTeams();

	if (teams.size()>0) 
	{
		zval *teamsArray;
		TSRMLS_FETCH();
		MAKE_STD_ZVAL(teamsArray);
		array_init(teamsArray);

		for(size_t i = 0; i < teams.size(); i++)
		{
			if (teams[i].size()>0) {
				zval *teamArray;
				MAKE_STD_ZVAL(teamArray);
				array_init(teamArray);

				RXTeam::const_iterator it = teams[i].begin();
				RXTeam::const_iterator end = teams[i].end();

				for(;it!=end;++it) {
					const RXPlayer *pl = *it;
					char *stupidAPI = strdup(pl->getName().c_str());
					add_next_index_string(teamArray, stupidAPI, 1);
					free(stupidAPI);
				}
				add_next_index_zval(teamsArray, teamArray);
			}
		}
		add_property_zval(return_val, "Teams",teamsArray);
	}

	addString(return_val, "Matchup", rep.getMatchup().c_str());
}



//=====================================================
// Version info
//=====================================================

static void addVersionInfo(const RXReplay &replay, zval *return_val) 
{
	TSRMLS_FETCH();
	zval *versionObject;
	MAKE_STD_ZVAL(versionObject);
	object_init(versionObject);

	const RXVersion *version = replay.getVersion();

	add_property_long(versionObject,"Version"     ,(uint)version->getVersion());
	add_property_long(versionObject,"Subversion"  ,(uint)version->getSubVersion());
	add_property_long(versionObject,"Revision"    ,version->getRevision());
	add_property_long(versionObject,"Code"		 ,version->getCode());

	addString        (versionObject,"VersionName"  ,version->getFormatedVersion().c_str());

	add_property_zval(return_val, "Version",versionObject);
}




//=====================================================
// Map info
//=====================================================

static void addMapInfo(const RXMapInfo *mapInfo, zval *return_val) 
{
    TSRMLS_FETCH();
    zval *mapObject;
    MAKE_STD_ZVAL(mapObject);
    object_init(mapObject);

    addString(mapObject,"Name"                  ,mapInfo->getName().c_str());
    add_property_long(mapObject,"Width"         ,(uint)mapInfo->getWidth());
    add_property_long(mapObject,"Height"        ,(uint)mapInfo->getHeight());
    add_property_long(mapObject,"Tileset"	    ,(uint)mapInfo->getTileSet());
    addString(mapObject,"TilesetName"	        ,mapInfo->getTileSetName().c_str());

	const std::vector<int> startLocs = mapInfo->getStartLocs();

	if(startLocs.size() > 0)
	{
		zval *locsArray;
		TSRMLS_FETCH();
		MAKE_STD_ZVAL(locsArray);
		array_init(locsArray);

		for(int i = 0; i < startLocs.size(); i++)
			add_next_index_double(locsArray, startLocs[i]);

		add_property_zval(mapObject, "StartingLocations", locsArray);
	}


    add_property_zval(return_val, "Map",mapObject);
}


/*
********************************
*			LOADERS
********************************
*/


//=====================================================
// Error
//=====================================================


static void addErrorInfo(int res, zval *return_val) 
{
	TSRMLS_FETCH();
	add_property_long(return_val,"ErrorCode",res);

	if (res == 0) 
		addString        (return_val,"ErrorString","OK");
	else if (res == -1)
		addString        (return_val,"ErrorString","Unable to load .rep file");
	else if (res == -2)
		addString        (return_val,"ErrorString","Unable to create temporary file to extract map");
	else if (res == -3)
		addString        (return_val,"ErrorString","Unable to create map (disk full ?)");
	else
		addString        (return_val,"ErrorString",strerror(res));
}


//=====================================================
// Replay
//=====================================================

static void loadReplay(const char *fileName, zval *return_val)
{

	BWrepFile replay;

	int res = replay.Load(fileName);
	addErrorInfo(res, return_val);

	if (res)
		return;
 
	try
	{
		RXReplay rxreplay(replay);
		addGlobalInfo(rxreplay,return_val);
		int RWTStatus = getRWTStatus(fileName);
		TSRMLS_FETCH();
		switch(RWTStatus)
		{
			case 1:
				add_property_long(return_val, "IsFPREP", 1);
				add_property_long(return_val, "IsRWT", 1);
				break;
			case 2:
				add_property_long(return_val, "IsFPREP", 0);
				add_property_long(return_val, "IsRWT", 1);
				break;
			case 0:
			case 3:
			default:
				add_property_long(return_val, "IsFPREP", 0);
				add_property_long(return_val, "IsRWT", 0);
				break;
		}
		addPlayerInfo(rxreplay,return_val);
		addTeamInfo(rxreplay, return_val);
		addVersionInfo(rxreplay, return_val);
		addMapInfo(rxreplay.getMapInfo(), return_val);
	}
	catch(const std::exception & re)
	{
		TSRMLS_FETCH();
		add_property_long(return_val, "ErrorCode", -1);
		addString(return_val, "ErrorString", re.what());
	}
	catch(...)
	{
		TSRMLS_FETCH();
		add_property_long(return_val, "ErrorCode", -1);
		addString(return_val, "ErrorString", "unknown error");
	}
}


//=====================================================
// Map
//=====================================================

static void loadSCM(const char *fileName, zval *return_val)
{
    try
    {
		RXMap *map = getCHKMap(fileName);
        RXMapInfo   mapInfo(map);

        addMapInfo(&mapInfo, return_val);

        addErrorInfo(0, return_val);
    }
    catch(const std::exception & re)
    {
        TSRMLS_FETCH();
        add_property_long(return_val, "ErrorCode", -1);
        addString(return_val, "ErrorString", re.what());
    }
    catch(...)
    {
        TSRMLS_FETCH();
        add_property_long(return_val, "ErrorCode", -1);
        addString(return_val, "ErrorString", "unknown error");
    }
}

//=====================================================
// Actions
//=====================================================

static void getAllActions(RXActionCompiler &compiler, int actionType, zval *return_val)
{
	RXActionsList actions = compiler.getActionsForRange(0, 0, actionType);
	
	if(actions.size() > 0)
	{
		zval *actionsArray;
		TSRMLS_FETCH();
		MAKE_STD_ZVAL(actionsArray);
		array_init(actionsArray);

		for(size_t i = 0; i < actions.size(); i++)
		{
			zval *zAction;
			TSRMLS_FETCH();
			MAKE_STD_ZVAL(zAction);
			object_init(zAction);

			const BWrepAction curAction = actions[i];
			addString(zAction, "Name", curAction.GetName());
			add_property_long(zAction, "Time", BWrepAction::Tick2Sec(curAction.GetTime()));
			addString(zAction, "Params", curAction.GetParameters());
			add_next_index_zval(actionsArray, zAction);
		}

		add_property_zval(return_val, "Actions", actionsArray);
	}
}

static void getActionsFrequency(RXActionCompiler &compiler, int actionType, int frequency, zval *return_val)
{
	RXRangedActionsList actions = compiler.getActionsAtFrequency(frequency, actionType);

	if(actions.size() > 0)
	{
		zval *actionsArray;
		TSRMLS_FETCH();
		MAKE_STD_ZVAL(actionsArray);
		array_init(actionsArray);

		for(size_t i = 0; i < actions.size(); i++)
		{
			if (actions[i].size()>0)
			{
				zval *actArray;
				MAKE_STD_ZVAL(actArray);
				array_init(actArray);

				for(size_t j = 0; j < actions[i].size(); j++)
				{
					zval *zAction;
					TSRMLS_FETCH();
					MAKE_STD_ZVAL(zAction);
					object_init(zAction);

					const BWrepAction curAction = actions[i][j];
					addString(zAction, "Name", curAction.GetName());
					add_property_long(zAction, "Time", BWrepAction::Tick2Sec(curAction.GetTime()));
					addString(zAction, "Params", curAction.GetParameters());
					add_next_index_zval(actArray, zAction);
				}
				add_next_index_zval(actionsArray, actArray);
			}
		}
		add_property_zval(return_val, "Actions", actionsArray);
	}
}


static void getActions(const char *fileName, const char* playerName, int actionType, int frequency, zval *return_val)
{
	BWrepFile replay;

	int res = replay.Load(fileName);
	addErrorInfo(res, return_val);

	if (res)
		return;

	try
	{
		RXActionCompiler compiler(replay, playerName);
		if(!compiler.isPlayerFound())
		{
			TSRMLS_FETCH();
			add_property_long(return_val, "ErrorCode", -2);
			std::string errorMsg = "Unable to find player ";
			errorMsg += playerName;
			addString        (return_val, "ErrorString", errorMsg.c_str());
			return;
		}

		// Gets actions, not APM
		if(actionType != -1)
		{
			if(frequency > 0)
				getActionsFrequency(compiler, actionType, frequency, return_val);
			else
				getAllActions(compiler, actionType, return_val);
		}
		// Gets APM
		else
		{
			std::vector<int> apmList = compiler.getAPMWithFrequency(frequency);
			if(apmList.size() > 0)
			{
				zval *apmArray;
				TSRMLS_FETCH();
				MAKE_STD_ZVAL(apmArray);
				array_init(apmArray);

				for(size_t i = 0; i < apmList.size(); i++)
				{
					TSRMLS_FETCH();
					add_index_long(apmArray, i, apmList[i]);
				}
				add_property_zval(return_val, "APM", apmArray);
			}
		}
	}
	catch(const std::exception & re)
	{
		TSRMLS_FETCH();
		add_property_long(return_val, "ErrorCode", -1);
		addString(return_val, "ErrorString", re.what());
	}
	catch(...)
	{
		TSRMLS_FETCH();
		add_property_long(return_val, "ErrorCode", -1);
		addString(return_val, "ErrorString", "unknown error");
	}
}





//=====================================================
// PHP mess
// Sponsorized by #define
//
//  " My dad used to code with so poorly designed APIs " - Lucy
//  
//=====================================================


extern "C" {

	PHP_MINFO_FUNCTION(php_bw_mod);
	ZEND_FUNCTION(php_bw_load_replay); 
    ZEND_FUNCTION(php_bw_load_scm);

    
	ZEND_FUNCTION(php_bw_jpg_from_scm_ratio); 
	ZEND_FUNCTION(php_bw_jpg_from_scm_dim); 


	ZEND_FUNCTION(php_bw_jpg_from_rep_ratio); 
	ZEND_FUNCTION(php_bw_jpg_from_rep_dim);

	ZEND_FUNCTION(php_bw_get_actions);

	ZEND_FUNCTION(php_bw_extract_map);
	


	/* function list so that the Zend engine will know what’s here */ 
	zend_function_entry php_bw_mod_functions[] = { 
		ZEND_FE(php_bw_load_replay, NULL)
        ZEND_FE(php_bw_load_scm, NULL)
		ZEND_FE(php_bw_jpg_from_scm_ratio, NULL)
		ZEND_FE(php_bw_jpg_from_scm_dim, NULL)
		ZEND_FE(php_bw_jpg_from_rep_ratio, NULL)
		ZEND_FE(php_bw_jpg_from_rep_dim, NULL)
		ZEND_FE(php_bw_get_actions, NULL)
		ZEND_FE(php_bw_extract_map, NULL)
		{NULL, NULL, NULL}
	};

	
	static PHP_MINIT_FUNCTION(php_bw_mod)
	{
		REGISTER_LONG_CONSTANT("RENDER_NOTHING", RXMinimap::RENDER_NOTHING, CONST_CS | CONST_PERSISTENT);
		REGISTER_LONG_CONSTANT("RENDER_DOODADS", RXMinimap::RENDER_DOODADS, CONST_CS | CONST_PERSISTENT);
		REGISTER_LONG_CONSTANT("RENDER_RESOURCES", RXMinimap::RENDER_RESOURCES, CONST_CS | CONST_PERSISTENT);
		REGISTER_LONG_CONSTANT("RENDER_STARTING_LOCATIONS", RXMinimap::RENDER_STARTING_LOCATIONS, CONST_CS | CONST_PERSISTENT);
		REGISTER_LONG_CONSTANT("RENDER_UNITS", RXMinimap::RENDER_BUILDINGS, CONST_CS | CONST_PERSISTENT);
		REGISTER_LONG_CONSTANT("RENDER_ALL", RXMinimap::RENDER_ALL, CONST_CS | CONST_PERSISTENT);
		REGISTER_LONG_CONSTANT("REPASM_HIGH_QUALITY", 1, CONST_CS | CONST_PERSISTENT);
		REGISTER_LONG_CONSTANT("REPASM_LOW_QUALITY", 0, CONST_CS | CONST_PERSISTENT);

		REGISTER_LONG_CONSTANT("REPASM_ALL_ACTIONS", RXActionCompiler::ALL, CONST_CS | CONST_PERSISTENT);
		REGISTER_LONG_CONSTANT("REPASM_BUILD_ACTIONS", RXActionCompiler::BUILD, CONST_CS | CONST_PERSISTENT);
		REGISTER_LONG_CONSTANT("REPASM_HOTKEY_ACTIONS", RXActionCompiler::HOTKEY, CONST_CS | CONST_PERSISTENT);
		REGISTER_LONG_CONSTANT("REPASM_RESEARCH_ACTIONS", RXActionCompiler::RESEARCH, CONST_CS | CONST_PERSISTENT);
		REGISTER_LONG_CONSTANT("REPASM_UPGRADE_ACTIONS", RXActionCompiler::UPGRADE, CONST_CS | CONST_PERSISTENT);
		REGISTER_LONG_CONSTANT("REPASM_APM", -1, CONST_CS | CONST_PERSISTENT);

		return SUCCESS;
	}


	/* module information */ 
	zend_module_entry php_bw_module_entry = 
	{ 
		STANDARD_MODULE_HEADER, "RepASM",
		php_bw_mod_functions, 
		PHP_MINIT(php_bw_mod), NULL, NULL, NULL, PHP_MINFO(php_bw_mod), 
		REPASM_VERSION, STANDARD_MODULE_PROPERTIES 
	};

	ZEND_GET_MODULE(php_bw);

	PHP_MINFO_FUNCTION(php_bw_mod)
	{
		php_info_print_table_start();
		php_info_print_table_header(2, "StarCraft Replay Parser", "enabled");
		php_info_print_table_row(2, "Version", REPASM_VERSION);
		php_info_print_table_row(2, "Minimap extraction", "available");
		php_info_print_table_end();
	}

	ZEND_FUNCTION(php_bw_load_replay) 
	{
		char *s;
		zval *param;

		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &s,&param) == FAILURE) 
			return;

		object_init(return_value);

		loadReplay(s, return_value);
	} 


    ZEND_FUNCTION(php_bw_load_scm) 
    {
        char *s;
        zval *param;

        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &s,&param) == FAILURE) 
            return;

        object_init(return_value);

        loadSCM(s, return_value);
    } 


  
	ZEND_FUNCTION(php_bw_jpg_from_scm_ratio) 
	{
		char *datpath, *scmPath,*destPath;
		int datLen, scmlen, destLen;
		int ratio=1;
		int quality=0;
		int jpegQuality=90;
		int renderMode=RXMinimap::RENDER_ALL;

		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssl|lll", &datpath,&datLen,
																	 &scmPath,&scmlen,
																	 &destPath,&destLen,
																	 &ratio,
																	 &quality,
																	 &jpegQuality,
																	 &renderMode
																	 ) == FAILURE) 
		{
			return;
		}

		object_init(return_value);


		try
		{
			generateMinimapRatioFromScm(datpath, scmPath, destPath, (float)ratio, quality,jpegQuality, renderMode);
			add_property_long(return_value, "ErrorCode", 0);
		}
		catch(const std::exception &re)
		{
			add_property_long(return_value, "ErrorCode", -1);
			addString(return_value, "ErrorString", re.what());
		}
		catch(...)
		{
			TSRMLS_FETCH();
			add_property_long(return_value, "ErrorCode", -1);
			addString(return_value, "ErrorString", "unknown error");
		}

	} 


	ZEND_FUNCTION(php_bw_jpg_from_scm_dim) 
	{
		char *datpath, *scmPath,*destPath;
		int datLen, scmlen, destLen;
		int maxDim=128;
		int quality=0;
		int jpegQuality=90;
		int renderMode=RXMinimap::RENDER_ALL;

		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssl|lll", &datpath,&datLen,
			&scmPath,&scmlen,
			&destPath,&destLen,
			&maxDim,
			&quality,
			&jpegQuality,
			&renderMode
			) == FAILURE) 
		{
			return;
		}

		object_init(return_value);

		try
		{
			generateMinimapDimFromScm(datpath, scmPath, destPath, maxDim, quality,jpegQuality, renderMode);
			add_property_long(return_value, "ErrorCode", 0);
		}
		catch(const std::exception &re)
		{
			add_property_long(return_value, "ErrorCode", -1);
			addString(return_value, "ErrorString", re.what());
		}
		catch(...)
		{
			TSRMLS_FETCH();
			add_property_long(return_value, "ErrorCode", -1);
			addString(return_value, "ErrorString", "unknown error");
		}
	} 









	ZEND_FUNCTION(php_bw_jpg_from_rep_ratio) 
	{
		char *datpath, *scmPath,*destPath;
		int datLen, scmlen, destLen;
		int ratio=1;
		int quality=0;
		int jpegQuality=90;
		int renderMode=RXMinimap::RENDER_ALL;

		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssl|lll", &datpath,&datLen,
			&scmPath,&scmlen,
			&destPath,&destLen,
			&ratio,
			&quality,
			&jpegQuality,
			&renderMode
			) == FAILURE) 
		{
			return;
		}

		object_init(return_value);


		try
		{
			generateMinimapRatioFromRep(datpath, scmPath, destPath, (float)ratio, quality,jpegQuality, renderMode);
			add_property_long(return_value, "ErrorCode", 0);
		}
		catch(const std::exception &re)
		{
			add_property_long(return_value, "ErrorCode", -1);
			addString(return_value, "ErrorString", re.what());
		}
		catch(...)
		{
			TSRMLS_FETCH();
			add_property_long(return_value, "ErrorCode", -1);
			addString(return_value, "ErrorString", "unknown error");
		}

	} 


	ZEND_FUNCTION(php_bw_jpg_from_rep_dim) 
	{
		char *datpath, *repPath,*destPath;
		int datLen, replen, destLen;
		int maxDim=128;
		int quality=0;
		int jpegQuality=90;
		int renderMode=RXMinimap::RENDER_ALL;

		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssl|lll", &datpath,&datLen,
			&repPath,&replen,
			&destPath,&destLen,
			&maxDim,
			&quality,
			&jpegQuality,
			&renderMode
			) == FAILURE) 
		{
			return;
		}

		object_init(return_value);

		try
		{
			generateMinimapDimFromRep(datpath, repPath, destPath, maxDim, quality, jpegQuality, renderMode);
			add_property_long(return_value, "ErrorCode", 0);
		}
		catch(const std::exception &re)
		{
			add_property_long(return_value, "ErrorCode", -1);
			addString(return_value, "ErrorString", re.what());
		}
		catch(...)
		{
			TSRMLS_FETCH();
			add_property_long(return_value, "ErrorCode", -1);
			addString(return_value, "ErrorString", "unknown error");
		}
	}

	ZEND_FUNCTION(php_bw_get_actions) 
	{
		char *fileName, *playerName;
		int fileNameLen, playerNameLen, freq = 10;
		int actionType = RXActionCompiler::ALL;

		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|ll", &fileName, &fileNameLen, &playerName, &playerNameLen, &actionType, &freq) == FAILURE) 
			return;

		object_init(return_value);

		getActions(fileName, playerName, actionType, freq, return_value);
	}

	ZEND_FUNCTION(php_bw_extract_map) 
	{
		char *repPath, *destPath;
		int repLen, destLen;

		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &repPath, &repLen, &destPath, &destLen) == FAILURE) 
			return;

		object_init(return_value);

		int res = RXMapExtractor::extractTo(repPath, destPath);
		addErrorInfo(res, return_value);
	}
}


