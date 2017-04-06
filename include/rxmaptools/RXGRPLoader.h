#ifndef RX_RGPLOADER
#define RX_RGPLOADER

#include <memory>
#include <misc/RXHashmap.h>

#include <rxmaptools/file/RXTBL.h>
#include <rxmaptools/file/RXDAT.h>

class RXGRP;
class RXTBL;
class RXFileManager;

class RXGRPLoader
{
public:
	RXGRPLoader(RXFileManager *fm);
	~RXGRPLoader(void);


	const RXGRP * getDoodads(int spriteID);
	const RXGRP * getDoodadsShadow(int spriteID);
	const RXGRP * getUnits(int unitID);
	const RXGRP * getUnitsShadow(int unitID);

	const RXGRP * getImage(int id);

private:

	RXFileManager *fm;

	typedef RXDAT<unsigned short>  RXTFlingyDat;
	typedef RXDAT<unsigned short> RXTSpriteDat;
	typedef RXDAT<unsigned int>   RXTImageDat;
	typedef RXDAT<unsigned char>  RXTUnitsDat;


	std::auto_ptr<RXTUnitsDat>  units;
	std::auto_ptr<RXTFlingyDat> flingy;
	std::auto_ptr<RXTSpriteDat> sprite;
	std::auto_ptr<RXTImageDat>  images;
	std::auto_ptr<RXTBL>		tbl;


	class RXTGRPHash : public RXHashNamespace::hash_map<int, RXGRP *>
	{
	public:
		~RXTGRPHash();
	};



	RXGRP *getFromTBLIndex(int tblid, bool allFrame);
	RXTGRPHash grpHash; //TBLEntry=>GRP

};


#endif






