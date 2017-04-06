#include "stdafx.h"
#include <stdio.h>
#include "BWTools/RXVersion.h"


RXVersion::RXVersion()
{
    set(1,0,0);
}

RXVersion::~RXVersion(void)
{
}








void RXVersion::set(int version, int subversion, char revision) {

	this->version		= version;
	this->subversion	= subversion;
	this->revision		= revision;

	this->code = (version << 16) | (subversion << 8) | revision;
	buildString();
}

void RXVersion::buildString()
{
	char temp[12];

    if (this->revision)
	{
		if(this->subversion >= 15)
			sprintf(temp, "%d.%.2d.%c",this->version, this->subversion, this->revision);
		else
			sprintf(temp, "%d.%.2d%c",this->version, this->subversion, this->revision);
	}
    else
		sprintf(temp, "%d.%.2d",this->version, this->subversion);

	formVersionName = temp;
}




