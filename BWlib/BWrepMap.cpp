//----------------------------------------------------------------------------------------------------
// Replay Map - jca (May 2003)
//----------------------------------------------------------------------------------------------------
#include "stdafx.h"
#include "BWlib/BWrepAPI.h"
#include "BWlib/BWrepActions.h"
#include "BWlib/BWrepGameData.h"
#include "BWlib/BWrepMap.h"

#include "unpack.h"
#include <assert.h>
#include <string.h>


//Added by Chrisbk 16-04-2005 for improved portability
//
// stricmp is visual C++ specific, while strcasecmp seems to be gcc specific
// With this quick define we ensure portability on both compilers. If you plan to use
// another compiler, you may have to change this define

#if _MSC_VER > 800
#define strcasecmp _stricmp
#endif


//------------------------------------------------------------------------------------------------------------

//
bool BWrepMap::DecodeMap(const unsigned char *buffer, int mapSize, int w, int h)
{
	const unsigned char *current = buffer;
	int read=0;

	// map dimensions
	m_mapWidth=w;
	m_mapHeight=h;

	// clear previous infos
	_Clear();

	// read block chain
	while(read<mapSize && m_sectionCount<MAXSECTION)
	{
		// extract block title
		char blockTitle[5];
		memcpy(blockTitle,current,4);
		blockTitle[4]=0;
		current+=4;

		// check for corruption
		//if(blockTitle[0]==0) break;
		//Removed by Chrisbk - this 'anti corruption' cheats prevented sometimes from loading chk

		// extract block size
		unsigned long bsize = *((unsigned long*)current);
		current+=4;

		// check for corruption

		//Modified by chrisbk. Test is now a bit more robust

		read+=8;
		if((read + bsize)>(unsigned long)mapSize) break;

		// init section block
		m_sections[m_sectionCount].Init(blockTitle,bsize,current);

		// next block
		m_sectionCount++;
		current+=bsize;
		read+=bsize;
	}

	// keep pointer on buffer
	m_data = buffer;
	m_datasize = mapSize;

	return true;
}

//------------------------------------------------------------------------------------------------------------

void BWrepMap::_Clear() 
{
	m_sectionCount=0;
	m_datasize=0;

	// free data buffer
	if(m_data!=0) free((void*)m_data);
	m_data=0;
}

//------------------------------------------------------------------------------------------------------------

BWrepMap::~BWrepMap() 
{
	_Clear();
}

//------------------------------------------------------------------------------------------------------------

// find section by name
const BWrepMapSection* BWrepMap::GetSection(const char *name) const
{
	for(int i=0; i<m_sectionCount; i++)
	{
		if(strcasecmp(name,m_sections[i].GetTitle())==0)
			return &m_sections[i];
	}
	return 0;
}

//------------------------------------------------------------------------------------------------------------

// get tile section info (2 bytes per map square)
const BWrepMapSection* BWrepMap::GetTileSection() const
{
	// depends on the replay file format
	const BWrepMapSection *tile= GetSection(SECTION_TILE);
	if(tile==0) tile = GetSection(SECTION_MTXM);
	return tile;
}

//------------------------------------------------------------------------------------------------------------

