#ifndef RX_SCMPQREADER__H
#define RX_SCMPQREADER__H

#include "RXFileManager.h"
#include <deque>
#include <string>

class RXMPQReader;

class RXSCMpqReader : public RXFileManager
{
public:
	enum RXDatType
	{ 
		USE_STARDAT=1, 
		USE_BROODAT=2,

		USE_ALL = USE_STARDAT | USE_BROODAT
	};

#ifdef _MSC_VER
	RXSCMpqReader(RXDatType type);
	RXSCMpqReader(RXDatType type, const std::wstring &basePath);
#endif

	RXSCMpqReader(RXDatType type, const std::string &basePath);


	~RXSCMpqReader(void);

	bool isLoaded(RXDatType type) const;

	

private:

	class RXMQReaderList : public std::deque<RXMPQReader *> 
	{
	public:
		~RXMQReaderList();
	};


	RXDatType loaded;
	RXMQReaderList reader;

//	void openReaders(RXDatType type, const std::wstring &basePath);
//	void openReader(const std::wstring &basePath, RXDatType type);

	template <typename T>
	void openReaders(RXDatType type, const T &basePath);

#ifdef _MSC_VER
	void openReader(const std::wstring &basePath, RXDatType type);
#endif

	void openReader(const std::string &basePath, RXDatType type);



	virtual std::istream * pGetFile(const std::string & name);
};

#endif

