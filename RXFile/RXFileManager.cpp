#include "stdafx.h"
#include <rxfile/RXFileManager.h>



RXFileNotFoundException::RXFileNotFoundException(const std::string &fName) :
std::runtime_error((std::string("Unable to load file ")+fName).c_str())
{
}


RXFileManager::RXFileManager(void)
{
}

RXFileManager::~RXFileManager(void)
{
}


std::istream * RXFileManager::getFile(const std::string &name)
{
	std::istream * res = pGetFile(name);

	if (res == NULL)
		throw RXFileNotFoundException(name);

	return res;
}



std::auto_ptr<std::istream> RXFileManager::getAutoFile(const std::string &name) 
{
	return std::auto_ptr<std::istream>(getFile(name));
}



unsigned int RXFileManager::getSize(std::istream *i)
{
	std::streampos p = i->tellg();

	i->seekg(0, std::ios::beg);
	unsigned int startPos = i->tellg(); //I know, this looks stupid but gcc2.95 is returning me some weird still coherent number

	i->seekg(0, std::ios::end);

	unsigned int res = (unsigned int) i->tellg();
	//i->seekg(p, std::ios::beg);
	i->seekg(0, std::ios::beg);

	return res-startPos;
}







