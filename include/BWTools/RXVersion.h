#ifndef __RX____VERSION
#define __RX____VERSION


#include <string>

class RXVersion
{
public:
	~RXVersion(void);



	int  getVersion()	 const { return version;}
	int  getSubVersion() const { return subversion;}
	char getRevision()   const { return revision;}
	int  getCode()		 const { return code;}

	const std::string & getFormatedVersion() const {return formVersionName;}

protected:
	RXVersion();
	void set(int version, int subversion, char revision);

private:

	int version;
	int subversion;
	char revision;

	unsigned int code;


	void buildString();
	
	std::string formVersionName;
};


#endif
