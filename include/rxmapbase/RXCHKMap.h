#ifndef RX_CHKMAP__H
#define RX_CHKMAP__H


#include "RXMap.h"

#include <iostream>

class RXCHKMap :
	public RXMap
{
public:
	RXCHKMap(std::istream *input);
	~RXCHKMap(void);


private:
	void read(std::istream *input);

	void readAndAddRXMapSection(std::istream *input, const char *sectionName, int size);

	template<typename T>
	void readAndAddSection(std::istream *input, const char *sectionName, int size);
};

#endif
