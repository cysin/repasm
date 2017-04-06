#ifndef RXMPQ_READER__H
#define RXMPQ_READER__H

#include "RXFileManager.h"
#include "StormLib.h"
#include <string>


class RXMPQReader : public RXFileManager
{
public:
#ifdef _MSC_VER
	RXMPQReader(const std::wstring &mpqName);
#endif
	RXMPQReader(const std::string &mpqName);
	~RXMPQReader(void);

protected:
	virtual std::istream * pGetFile(const std::string &name);

private:
	HANDLE hMPQ;
};

#endif
