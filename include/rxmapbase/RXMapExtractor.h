#pragma once

#include <string>

class BWrepFile;

class RXMapExtractor
{
public:

	/**
	* Extracts the map from a replay.
	* @param from The path to the replay (e.g. E:\StarCraft\maps\replays\MyReplay.rep)
	* @param to The path to the SCM/SCX file (e.g. E:\StarCraft\maps\MyMap.scm)
	* @return 0 if OK, -1 if replay could not be loaded, -2 if temp file could not be created, -3 if final file could not be created.
	**/
    static int extractTo(const std::string &from, const std::string &to);

#ifdef _WIN32
    static int extractTo(const std::wstring &from, const std::wstring &to);
#endif


private:
    RXMapExtractor(void);
    ~RXMapExtractor(void);

    static std::string dumpToTempFileA(BWrepFile &f);
    static std::string createTempFileA();

    static bool createSCM(const std::string &to, std::string &chk);
};
