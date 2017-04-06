#ifndef RX_GRP
#define RX_GRP

#include <iostream>
#include <deque>

class RXGRP
{
public:
	RXGRP(std::istream *input, bool allFrame=false);
	~RXGRP(void);

	int						getFrameCount()			const { return (int)bitmap.size();}
	const unsigned char *	getFrameBitmap(int i=0) const {
		if((size_t)i < bitmap.size())
			return bitmap[i];
		else
			return NULL;}

	int getWidth()  const { return width;}
	int getHeight() const { return height;}


private:

	int width, height;
	std::deque<unsigned char *> bitmap;



	struct GRPFrameHeader
	{
		unsigned char   xOffset, yOffset;
		unsigned short unknown;
		unsigned int   offset;
	};

	struct GRPHeader
	{
		unsigned short frameCount;
		unsigned short width;
		unsigned short height;

		std::deque<GRPFrameHeader> frameHeader;
	};

	struct GRPFrameLineOffset
	{
		unsigned short *offset;
		int numLine;
		int start;
	};

	void			 load(std::istream *input, bool allFrame);
	void			 readHeader(std::istream *input, GRPHeader &header);
	unsigned char *  readFrame(std::istream *input, GRPHeader &header, int frame);

	void getLineOffset(std::istream *input, GRPHeader &header, int frame, GRPFrameLineOffset &offset);

	//Gcc 2.95 returns crap when calling istringstream::tellg
	//here's a hack to bypass this
	unsigned int streamPos0;
};


#endif 
