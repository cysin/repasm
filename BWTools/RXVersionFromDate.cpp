#include "stdafx.h"
#include "RXVersionFromDate.h"

#include <fstream>
#include <sstream>
#include <ios>


RXVersionFromDate::RXVersionFromDate()
{
	initDefault();
}

RXVersionFromDate::RXVersionFromDate(const std::string &filePath)
{
	// If this one fails, at least we can use the default
	if(!initFromFile(filePath))
		initDefault();
}

RXVersionFromDate::~RXVersionFromDate(void)
{
}

void RXVersionFromDate::buildFromDate(time_t gamedate)
{
	if (gamedate<=0)		//fail safe, I have one replay with negative time stamp (?!)
		gamedate = 1; 

	int i=0;

	int size = knownVersion.size();
	while(gamedate < knownVersion[i].stamp && i < size - 1)
	{
		time_t curStamp = knownVersion[i].stamp;
		i++;
	}

	set( knownVersion[i].version,  knownVersion[i].subversion,  knownVersion[i].revision);
}


void RXVersionFromDate::initDefault()
{
	//All known version of Starcraft with their timestamp
	KnownVersion localKnownVersion[] =
	{
		{1227567600, 1, 16, 0},
		{1221148800, 1, 15, '3'},
		{1200438000, 1, 15, '2'},
		{1187647200, 1, 15, '1'},
		{1179223200, 1, 15, 0},
		{1154383200, 1, 14, 0},
		{1137625200, 1, 13, 'f'},
		{1126562400, 1, 13, 'e'},
		{1126044000, 1, 13, 'd'},
		{1124748000, 1, 13, 'c'},
		{1123884000, 1, 13, 'b'},
		{1120082400, 1, 13, 0},

		{1109265420, 1, 12, 'b'},
		{1108601100, 1, 12, 0},

		{1086115140, 1, 11, 'b'},
		{1083197460, 1, 11, 0},

		{1049234400, 1, 10, 0},

		{1014666240, 1,  9, 'b'},
		{1012960860, 1,  9, 0},

		{990400320, 1,  8, 'b'},
		{990150540, 1,  8, 0},

		{941497200, 1,  7, 0},

		{0, 1, 0, 0},
	};

	for(int i = 0; i < 20; i++)
		knownVersion.push_back(localKnownVersion[i]);
}

bool RXVersionFromDate::initFromFile(const std::string &filePath)
{
	std::ifstream inStream(filePath.c_str(), std::ios_base::in);
	if(inStream.is_open())
	{
		std::string line;
		while(!(std::getline(inStream, line, '\n').eof()))
		{
			KnownVersion parsed = parseLine(line);
			if(parsed.stamp != 0 && parsed.version != 0 && parsed.subversion != 0)
				knownVersion.push_back(parsed);
		}

		inStream.close();
		if(knownVersion.size() > 0)
			return true;
	}

	return false;
}

/*
* Parses lines of text config files. A line must be : version=timestamp
* with version being "V.SVr" (Version, SubVersion, revision) or "V.SV.r".
*/
RXVersionFromDate::KnownVersion RXVersionFromDate::parseLine(const std::string &line)
{
	KnownVersion result;
	result.version = 0;
	result.subversion = 0;
	result.revision = '\0';
	result.stamp = 0;

	// First delimiter
	size_t found = line.find("=");
	if(found != std::string::npos)
	{
		// Now we have the timestamp
		std::string version = line.substr(0, found);
		std::string stamp = line.substr(found + 1);
		std::stringstream ss(stamp);

		// Extracts the version if everything is fine
		if(!(ss >> result.stamp).fail())
		{
			// First char must always be 1 unless Blizzard makes a big patch :]
			if(version[0] == '1' && version[1] == '.')
			{
				result.version = 1;
				// Now onto the subversion and revision
				// It's trickier because of the change in format in 1.15 : before we had something like "1.11b" but now it's "1.15.0"
				// Therefore we have to check if the subversion is >= 15
				std::string subver = version.substr(2, 2);
				std::stringstream subStr(subver);
				if(!(subStr >> result.subversion).fail() && version.length() > 4)
				{
					if(result.subversion >= 15)
						result.revision = version.substr(5, 1)[0];
					else
						result.revision = version.substr(4, 1)[0]; // yeah baby
				}
			}
		}
	}

	return result;
}
