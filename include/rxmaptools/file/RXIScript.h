#ifndef RX_ISCRIPT
#define RX_ISCRIPT

//read and manage the iscript.bin file
#include <iostream>

class RXIScript
{
public:
	RXIScript(std::istream *input);
	~RXIScript(void);

	enum RXScriptType
	{
		INITIAL
	};

	const unsigned char *getScript(int imageID, RXScriptType) const;

private:
	unsigned char *data;

#pragma pack(1)
	struct RXEntry
	{
		unsigned short id;
		unsigned short offset;
	};
#pragma pack()

#pragma pack(1)
	struct RXEntryHeader
	{
		char header[4];
		unsigned char type;
		char spacer[3];
	};
#pragma pack()

	struct RXPointer
	{
		unsigned short offsetToInitial;
	};



	const RXEntry *entry;

};



#endif












