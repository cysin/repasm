#ifndef __RXPLAYER__IMPORT
#define __RXPLAYER__IMPORT

#include <iostream>
#include "BWTools/RXPlayer.h"

class RXPlayerImport :
	public RXPlayer
{
public:
	RXPlayerImport(std::istream &);
	~RXPlayerImport(void);
};

#endif
