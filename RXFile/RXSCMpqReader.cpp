#include "stdafx.h"
#include <string>
#include <rxfile/RXSCMpqReader.h>
#include <rxfile/RXMPQReader.h>

#ifdef _MSC_VER
#include <rxpreferences/RXSCPathPreferences.h>
#endif




#ifdef _MSC_VER
RXSCMpqReader::RXSCMpqReader(RXDatType type)
{
	RXSCPathPreferences pref;
	pref.load();

	openReaders(type, pref.getSCDirectory());
}

RXSCMpqReader::RXSCMpqReader(RXDatType type, const std::wstring &basePath)
{
	openReaders<std::wstring>(type, basePath);
}
#endif



RXSCMpqReader::RXSCMpqReader(RXDatType type, const std::string &basePath)
{
	openReaders<std::string>(type, basePath);
}


RXSCMpqReader::~RXSCMpqReader()
{
}



template <typename T>
void RXSCMpqReader::openReaders(RXDatType type, const T &basePath)
{

	if (type & USE_BROODAT)
		openReader(basePath, USE_BROODAT);

	if (type & USE_STARDAT)
		openReader(basePath, USE_STARDAT);
}

void RXSCMpqReader::openReader(const std::string &basePath, RXDatType type)
{
	std::string name;

	switch(type)
	{
	case USE_BROODAT: name = "broodat.mpq"; break;
	case USE_STARDAT: name = "stardat.mpq"; break;
	}
	std::string path = basePath;

#ifdef _WIN32
	path.append("\\").append(name);
#else
	path.append("/").append(name);
#endif
	
	RXMPQReader *res = new RXMPQReader(path);

	if (res)
	{
		loaded=(RXDatType) (loaded | type);
		reader.push_back(res);
	}
}





#ifdef _MSC_VER
void RXSCMpqReader::openReader(const std::wstring &basePath, RXDatType type)
{
	std::wstring name;

	switch(type)
	{
	case USE_BROODAT: name = L"broodat.mpq"; break;
	case USE_STARDAT: name = L"stardat.mpq"; break;
	}
	std::wstring path = basePath;
	path.append(L"\\").append(name);

	RXMPQReader *res = new RXMPQReader(path);

	if (res)
	{
		loaded=(RXDatType) (loaded | type);
		reader.push_back(res);
	}
}
#endif






































std::istream * RXSCMpqReader::pGetFile(const std::string & name)
{
	for (size_t i=0;i<reader.size();i++)
	{
		try
		{
			std::istream *res = reader[i]->getFile(name);
			if (res)
				return res;
		}
		catch(const RXFileNotFoundException &)
		{
		}
	}

	return NULL;
}



bool RXSCMpqReader::isLoaded(RXDatType type) const
{
	return ((loaded & type)!=0);
}


RXSCMpqReader::RXMQReaderList::~RXMQReaderList() 
{
	for (size_t i=0;i<this->size();i++)
		delete (*this)[i];
}
