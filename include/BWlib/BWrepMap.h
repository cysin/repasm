//----------------------------------------------------------------------------------------------------
// Replay Map - jca (May 2003)
//----------------------------------------------------------------------------------------------------
// Classes you should use:
//
// class BWrepMap 
//----------------------------------------------------------------------------------------------------
#ifndef INC_BWREPMAP_H
#define INC_BWREPMAP_H

//#include "dllexports.h"
#include <string.h>
#pragma pack(1)

//----------------------------------------------------------------------------------------------------

// section info
class BWrepMapSection
{
public:
	const char *GetTitle() const {return m_title;}
	unsigned long GetSize() const {return m_size;}
	const unsigned char *GetData() const {return m_data;}

	//-internal
	void Init(const char *title, unsigned long size, unsigned const char *data)
		{memcpy(m_title,title,MAXTITLE); m_title[MAXTITLE]=0; m_size=size; m_data=data;}

private:
	enum {MAXTITLE=4};
	char m_title[MAXTITLE+1];
	unsigned long m_size;
	const unsigned char *m_data;     // pointer to data
};

//----------------------------------------------------------------------------------------------------

// Unit section info
class BWrepMapSectionUNIT : public BWrepMapSection
{
public:
	typedef enum
	{
		UNIT_STARTLOCATION=214,
		UNIT_MINERAL1=176,
		UNIT_MINERAL2=177,
		UNIT_MINERAL3=178,
		UNIT_GEYSER=188
	} eMAPUNITID;

	class BWrepUnitDesc
	{
	public:
		#pragma pack(1)
		unsigned short d1;
		unsigned short d2;
		unsigned short x; // x32
		unsigned short y; // x32
		unsigned short unitid;  // value from eMAPUNITID
		unsigned char bytes1[6];
		unsigned char playerid;
		unsigned char bytes2[3];
		unsigned short mineral; // for mineral or geyser
		unsigned char bytes3[14];
	};

	int GetUnitCount() const {return GetSize()/sizeof(BWrepUnitDesc);}
	BWrepUnitDesc* GetUnitDesc(int i) const {return (BWrepUnitDesc*)(GetData()+i*sizeof(BWrepUnitDesc));}
};

//----------------------------------------------------------------------------------------------------

#define SECTION_TILE "TILE"
#define SECTION_ISOM "ISOM"
#define SECTION_MTXM "MTXM"
#define SECTION_UNIT "UNIT"

// map info
class BWrepMap
{
public:
	BWrepMap() : m_sectionCount(0), m_data(0), m_datasize(0) {}
	~BWrepMap();

	// map dimensions
	int GetWidth() const {return m_mapWidth;}
	int GetHeight() const {return m_mapHeight;}

	// find section by name
	const BWrepMapSection* GetSection(const char *name) const;

	// get tile section info (2 bytes per map square)
	const BWrepMapSection* GetTileSection() const;


	//Added chrisbk 3/12/2005 : section enumeration
	int GetNumSection() const							 {return m_sectionCount;}
	const BWrepMapSection* GetSectionNumber(int i) const { return &m_sections[i];}


	
	//-internal
	bool DecodeMap(const unsigned char *buffer, int mapSize, int w, int h);



private:
	const unsigned char *m_data;     // pointer to data
	int m_datasize;	// data size

	// sections
	enum {MAXSECTION=128}; // Modified by Taiche : moved to MAXSECTION to 128 instead of 36 to avoid stopping reading sections if the CHK contains more than 36.
	BWrepMapSection m_sections[MAXSECTION];
	int m_sectionCount;

	// map dimensions
	int m_mapWidth;
	int m_mapHeight;

	// clear current info
	void _Clear();
};

//----------------------------------------------------------------------------------------------------
#pragma pack() //restore alignement
#endif // INC_BWREPMAP_H
