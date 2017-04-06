 #ifndef __RX__VERSION__FROMDATE
#define __RX__VERSION__FROMDATE

#include "BWTools/RXVersion.h"
#include <time.h>
#include <string>
#include <vector>

class RXVersionFromDate : public RXVersion
{
public:
	RXVersionFromDate();
	RXVersionFromDate(const std::string &filePath);
	~RXVersionFromDate(void);

	void buildFromDate(time_t gamedate);

private:
	struct KnownVersion 
	{
		time_t stamp;
		int version;
		int subversion;
		char revision;
	};
	typedef std::vector<KnownVersion> RXVersionList;

	RXVersionList knownVersion;

	void initDefault();
	bool initFromFile(const std::string &filePath);
	KnownVersion parseLine(const std::string &line);
};

#endif
