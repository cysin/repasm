#include "stdafx.h"
#include <rxmaptools/RXTeamPalette.h>

RXTeamPalette::RXTeamPalette(const RXPalette &src, int teamNumber)
{

	for (int i=0;i<256;i++)
		setColor(i, src[i]);

	int startCol = teamNumber * 8;
	for (int i=0;i<8;i++) //team color are stored in the [8-15] colors
		setColor(i+8, teamColor[startCol+i]);
}


RXTeamPalette::~RXTeamPalette(void)
{
}



const unsigned int *RXTeamPalette::getTeamColor(int teamNumber)
{
	return teamColor + 8 * teamNumber;
}


//Extracted from tunit.pcx
const unsigned int RXTeamPalette::teamColor[128] =
{
	0xFFf40404,
		0xFFa80808,
		0xFFa80808,
		0xFF840404,
		0xFF600000,
		0xFF480000,
		0xFF340000,
		0xFF100000,
		0xFF0c48cc,
		0xFF083498,
		0xFF083498,
		0xFF14347c,
		0xFF042064,
		0xFF000850,
		0xFF001034,
		0xFF000018,
		0xFF2cb494,
		0xFF209070,
		0xFF209070,
		0xFF209070,
		0xFF10543c,
		0xFF10543c,
		0xFF10543c,
		0xFF002800,
		0xFF88409c,
		0xFF88409c,
		0xFF88409c,
		0xFF683078,
		0xFF481c50,
		0xFF481c50,
		0xFF481c50,
		0xFF381020,
		0xFFf88c14,
		0xFFe87824,
		0xFFbc6824,
		0xFFa0541c,
		0xFF7c4018,
		0xFF5c2c14,
		0xFF34200c,
		0xFF1c1008,
		0xFF703014,
		0xFF5c2c14,
		0xFF5c2c14,
		0xFF443408,
		0xFF443408,
		0xFF34200c,
		0xFF341008,
		0xFF1c1008,
		0xFFcce0d0,
		0xFFc4c0bc,
		0xFFb0b0b0,
		0xFF98948c,
		0xFF847474,
		0xFF585868,
		0xFF444444,
		0xFF282830,
		0xFFfcfc38,
		0xFFdcdc3c,
		0xFFbcb834,
		0xFF9c9040,
		0xFF706420,
		0xFF5c5414,
		0xFF443408,
		0xFF1c1008,
		0xFF088008,
		0xFF006400,
		0xFF006400,
		0xFF006400,
		0xFF004400,
		0xFF004400,
		0xFF004400,
		0xFF002800,
		0xFFfcfc7c,
		0xFFfcfc7c,
		0xFFccb860,
		0xFFbca45c,
		0xFF9c9040,
		0xFF746838,
		0xFF684c34,
		0xFF383024,
		0xFFecc4b0,
		0xFFe0a484,
		0xFFbca45c,
		0xFFa89454,
		0xFF987454,
		0xFF846044,
		0xFF784c2c,
		0xFF5c2c14,
		0xFF4068d4,
		0xFF4068d4,
		0xFF00709c,
		0xFF00709c,
		0xFF14347c,
		0xFF14347c,
		0xFF042064,
		0xFF001034,
		0xFF74a47c,
		0xFF74a47c,
		0xFF68946c,
		0xFF508c5c,
		0xFF4c8050,
		0xFF406c3c,
		0xFF406c3c,
		0xFF344428,
		0xFF9090b8,
		0xFF9090b8,
		0xFF84849c,
		0xFF84849c,
		0xFF60607c,
		0xFF585868,
		0xFF405874,
		0xFF304054,
		0xFFfcfc7c,
		0xFFfcfc7c,
		0xFFccb860,
		0xFFbca45c,
		0xFF9c9040,
		0xFF746838,
		0xFF684c34,
		0xFF383024,
		0xFF00e4fc,
		0xFF00e4fc,
		0xFF00e4fc,
		0xFF18acb8,
		0xFF18acb8,
		0xFF00709c,
		0xFF10543c,
		0xFF10543c
};