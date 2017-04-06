#ifndef __RX_FSFILEMANAGER
#define __RX_FSFILEMANAGER 


#include "RXFileManager.h"

class RXFSFileManager :
	public RXFileManager
{
public:
	RXFSFileManager(const std::string &searchPath=".");
	~RXFSFileManager(void);




protected:
	virtual std::istream * pGetFile(const std::string &name);
private:
	std::string searchPath;
};

#endif
