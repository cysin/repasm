#ifndef RX_LOLOADER__H
#define RX_LOLOADER__H

#include <rxmaptools/file/RXTBL.h>
#include <misc/RXHashmap.h>
#include <iostream>

class RXLO;


class RXLOLoader
{
public:
	RXLOLoader(RXFileManager *fm);
	~RXLOLoader(void);

	const RXLO * loFromTBL(int tblID);

private:

	RXTBL tbl;
	RXFileManager *fm;

	typedef RXTPtrHash<int, RXLO> RXTLoHash;
	RXTLoHash loCache;
};

#endif
