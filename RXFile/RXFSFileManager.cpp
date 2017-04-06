#include "stdafx.h"
#include <rxfile/RXFSFileManager.h>
#include <fstream>


RXFSFileManager::RXFSFileManager(const std::string &sp) : searchPath(sp)
{
}

RXFSFileManager::~RXFSFileManager(void)
{
}



std::istream * RXFSFileManager::pGetFile(const std::string &name)
{
	std::string path(searchPath);

	path.append("\\").append(name);

	std::ifstream *str = new std::ifstream(path.c_str(),std::ios::in | std::ios::binary);

	if (str->is_open())
		return str;

	delete str;
	return NULL;
}
