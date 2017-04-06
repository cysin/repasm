#ifndef RX_IMAGEDATE_H
#define RX_IMAGEDATE_H

#include <iostream>
#include <rxmapbase/RXMapEnum.h>

class RXImageDAT
{
public:
	RXImageDAT(std::istream *str);
	~RXImageDAT(void);



	unsigned int getTblEntry(int id) const	{ return tblEntry[id];}
	unsigned int getScript(int id) const	{ return scriptID[id];}
	unsigned int getOverlay4(int id) const	{ return overlay4[id];}

	RXMapEnum::Palette getPalette(int id) const;


private:
	unsigned int  * tblEntry;
	unsigned char * palette;
	unsigned int  * scriptID;
	unsigned int  * overlay4;

	template <typename T>
		void readArray(std::istream *input, T *&dest, int num);

	void read(std::istream *input);
};

#endif
