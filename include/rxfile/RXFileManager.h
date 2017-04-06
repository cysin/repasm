#ifndef __RX__FILEMANAGER
#define __RX__FILEMANAGER

#include <iostream>
#include <string>
#include <memory>
#include <stdexcept>

class RXFileNotFoundException : public std::runtime_error
{
public:
	RXFileNotFoundException(const std::string &fName);
	~RXFileNotFoundException() throw(){};
};


class RXFileManager
{
public:
	RXFileManager();
	~RXFileManager(void);


	std::istream * getFile(const std::string &name);

	std::auto_ptr<std::istream> getAutoFile(const std::string &name);

	static unsigned int getSize(std::istream *);
protected:

	virtual std::istream * pGetFile(const std::string &name)=0;

};

#endif
