#ifndef RX_TBL
#define RX_TBL

#include <iostream>
#include <string>
#include <vector>


class RXTBL
{
public:
	RXTBL(std::istream *ifstr); 
	~RXTBL(void);


	size_t size() const { return str.size();}
	const char * operator[](int index)	const { return str[index];}

private:

	void read(std::istream *ifstr);

	std::vector<char *> str;
	char *pData;
};

#endif
