#include "stdafx.h"

#include <sstream>

#include <rxfile/RXMPQReader.h>
//#include <rxfile/RXStormlib.h>
//#include <file/RXStorm.h>

//#define MPQLIB RXStormlib



#ifdef _MSC_VER
#include <comutil.h>
#endif

#ifdef _MSC_VER
RXMPQReader::RXMPQReader(const std::wstring &mpqName)
{

	hMPQ = INVALID_HANDLE_VALUE;
	//if (MPQLIB::openArchive(mpqName,&hMPQ)==false)
	_bstr_t str = mpqName.c_str();
	if(!SFileOpenArchive((LPCSTR)str, 0, 0, &hMPQ) || hMPQ == NULL || hMPQ == INVALID_HANDLE_VALUE)
		throw RXFileNotFoundException((LPCSTR)_bstr_t(mpqName.c_str()));
}
#endif 


RXMPQReader::RXMPQReader(const std::string &mpqName)
{
	hMPQ = INVALID_HANDLE_VALUE;

	//if (MPQLIB::openArchive(mpqName,&hMPQ)==false)
	if(!SFileOpenArchive(mpqName.c_str(), 0, 0, &hMPQ) || hMPQ == NULL || hMPQ == INVALID_HANDLE_VALUE)
		throw RXFileNotFoundException(mpqName);
}


RXMPQReader::~RXMPQReader(void)
{
	if (hMPQ != INVALID_HANDLE_VALUE)
		SFileCloseArchive(hMPQ);
}


std::istream * RXMPQReader::pGetFile(const std::string &name)
{
	// Verif pour le cas où le Reader a plantai dans le constructeur
	if(hMPQ == INVALID_HANDLE_VALUE)
		return NULL;

	HANDLE hFile;

	//if (MPQLIB::openFile(hMPQ,name,&hFile)==false)
	if(!SFileOpenFileEx(hMPQ, name.c_str(), 0, &hFile))
		return NULL;


	DWORD fileSize = SFileGetFileSize(hFile, NULL);
	if (fileSize == 0xffffffff)
	{
		SFileCloseFile(hFile);
		return NULL;
	}


	std::istringstream * res = NULL;

	DWORD bytesRead;
	char *buffer = new char[fileSize];

	//if(MPQLIB::readFile(hFile, (void *)buffer, fileSize, &bytesRead) && bytesRead == fileSize)
	if(SFileReadFile(hFile, (void *)buffer, fileSize, &bytesRead) && bytesRead == fileSize)
		res = new std::istringstream(std::string(buffer, bytesRead), std::ios::in | std::ios::binary);

	delete []buffer;
	SFileCloseFile(hFile);

	return res;
}



