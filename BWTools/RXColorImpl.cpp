#include "stdafx.h"
#include "./RXColorImpl.h"
#include "BWlib/BWrepAPI.h"

RXColorImpl::RXColorImpl(const BWrepFile &file, const BWrepPlayer &player)
{
	ColorID colorID = getColorFromCode(file, player);

	// If the map doesn't have the COLR section, gets the normal color.
	if(colorID == -1)
		colorID = (ColorID)file.m_oHeader.GetPlayerColor(player.getNumber());

	buildWithID(colorID);
}

RXColorImpl::RXColorImpl(std::istream &str) {
	ColorID id;
	str.read((char *)&id, sizeof(id));
	buildWithID(id);
}


RXColorImpl::~RXColorImpl(void)
{
} 


void RXColorImpl::buildWithID(ColorID id) {
	this->colorCode = getHTMLColorFromCode(id);
	this->colorName = getColorNameFromCode(id);
	this->colorID   = id;
}


// Gets the right color according to the COLR section of the map in order to take custom colors into account.

RXColorImpl::ColorID RXColorImpl::getColorFromCode(const BWrepFile &file, const BWrepPlayer &player) {


	const BWrepMapSection *COLR = file.m_oMap.GetSection("COLR");
	int colorIdx = file.m_oHeader.GetPlayerColor(player.getNumber());

	if(COLR != NULL && colorIdx < 8)
	{
		const unsigned char * colorList = COLR->GetData();
		return (ColorID)colorList[colorIdx];
	}

	return (ColorID)-1;
}


const char * RXColorImpl::getColorNameFromCode(ColorID value) {

	switch(value) {
		case 0:     return "Red";
		case 1:     return "Blue";
		case 2:     return "Teal";
		case 3:     return "Purple";
		case 4:     return "Orange";
		case 5:     return "Brown";
		case 6:     return "White";
		case 7:     return "Yellow";
		case 8:     return "Green";
		case 9:     return "Pale yellow";
		case 10:    return "Tan";
		case 11:    return "Aqua";
		case 12:    return "Pale green";
		case 13:    return "Blueish gray";
		case 14:    return "Pale yellow";
		case 15:    return "Cyan";
		case 17:	return "Black";
		case 18:	return "Neon Blue";
		case 21:	return "Lavender";
		case 22:	return "Black";
		case 30:	return "Sky Blue";
		case 33:	return "Purple";

		default:
			return "Unsupported";
	}
}

int RXColorImpl::getHTMLColorFromCode(ColorID colorIdx)
{
	switch(colorIdx) {
		case 0:     return 0xFF0000;
		case 1:     return 0x0000FF;
		case 2:     return 0x209070;
		case 3:     return 0x88409C;
		case 4:     return 0xE87824;
		case 5:     return 0x5C2C14;
		case 6:     return 0xFFFFFF;
		case 7:     return 0xDCDC3C;
		case 8:     return 0x0F930F;
		case 9:     return 0xFCFC8F;
		case 10:    return 0xEFCEBD;
		case 11:    return 0x547CDC;

		default:
			return 0;
	}
}
