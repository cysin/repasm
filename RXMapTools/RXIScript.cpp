#include "stdafx.h"
#include <rxmaptools/file/RXIScript.h>
#include <rxfile/RXFileManager.h>

RXIScript::RXIScript(std::istream *input)
{
	int size = RXFileManager::getSize(input);

	data = new unsigned char [size];
	input->read((char *)data, size);


	//Set value
	unsigned short *offset = (unsigned short *)data;

	entry = (RXEntry *)(data + (*offset));
}




RXIScript::~RXIScript(void)
{
	delete []data;
}



const unsigned char *RXIScript::getScript(int imageID, RXIScript::RXScriptType) const
{
	int sizeHeader = sizeof(RXEntryHeader);

	int i=0;
	while(1)
	{
		if (entry[i].id == imageID)
			break;
		i++;
	}

	const RXPointer *pointerTable = (const RXPointer *)(data + entry[i].offset + sizeof(RXEntryHeader));
	unsigned char *script = data + pointerTable->offsetToInitial;

	return script;
}







