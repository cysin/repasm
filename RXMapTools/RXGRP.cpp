#include "stdafx.h"
#include <rxmaptools/file/RXGRP.h>



RXGRP::RXGRP(std::istream *input, bool allFrame)
{
	load(input, allFrame);
}

RXGRP::~RXGRP(void)
{
	for (size_t i=0;i<bitmap.size();i++)
		delete []bitmap[i];
}




void RXGRP::load(std::istream *input, bool allFrame)
{
	//anti gcc-2.95 istringstream::tellg bug
	streamPos0 = input->tellg();

	GRPHeader header;
	readHeader(input, header);

	int maxFrame = header.frameCount;
	if (allFrame == false)
		maxFrame = 1;

	for (int i=0;i<maxFrame;i++)
		bitmap.push_back(readFrame(input,header, i));

}





void RXGRP::readHeader(std::istream *input, GRPHeader &header)
{

	input->read((char *)&header.frameCount,2); 
	input->read((char *)&header.width,2); 
	input->read((char *)&header.height,2);

	width = header.width;
	height= header.height;

	for (int i=0;i<header.frameCount;i++)
	{
		GRPFrameHeader fh;

		input->read((char *)&fh.xOffset,1); 
		input->read((char *)&fh.yOffset,1); 
		input->read((char *)&fh.unknown,2);
		input->read((char *)&fh.offset,4);

		header.frameHeader.push_back(fh);
	}
}


unsigned char* RXGRP::readFrame(std::istream *input, GRPHeader &header, int frame)
{

	GRPFrameLineOffset lf;
	GRPFrameHeader &frameHeader = header.frameHeader[frame];



	unsigned char *dst = new unsigned char [header.width * header.height];
	unsigned char *res = dst;
	memset(dst, 0, header.width * header.height);

	getLineOffset(input, header, frame, lf);





	dst+=frameHeader.yOffset * header.width;

	


	for (int i=0;i<lf.numLine-1;i++)
	{
		int curXpos = frameHeader.xOffset;
		input->seekg(lf.start + lf.offset[i], std::ios::beg);


		while( ((int)input->tellg()-streamPos0)-lf.start <lf.offset[i+1]) 
		{ 
			unsigned char byte; 
			input->read((char *)&byte,1); 

			if (byte>=0x80) 
				curXpos += (byte - 0x80); 

			else if (byte >0x40) 
			{ 
				unsigned char byte2; 
				input->read((char *)&byte2,1); 

				for (int i=0;i<byte-0x40;i++) 
				{
					dst[curXpos]  =byte2;
					curXpos++;
				}
			} 
			else 
			{ 

				for (int j=0;j<byte;j++) 
				{ 
					unsigned char byte2;

					input->read((char *)&byte2,1); 
					dst[curXpos] =byte2;
					curXpos++; 
				} 
			} 
		} 

		if (curXpos > header.width)
			curXpos = curXpos;

		curXpos = frameHeader.xOffset; 

		dst+=header.width;
	} 

	delete []lf.offset;
	return res;
}




void RXGRP::getLineOffset(std::istream *input, RXGRP::GRPHeader &header, int frame, RXGRP::GRPFrameLineOffset &off)
{
	GRPFrameHeader &fHeader = header.frameHeader[frame];


	unsigned short sizeFrameHeader;

	//Lecture du nombre de ligne
	input->seekg(fHeader.offset,std::ios::beg);
	input->read((char *)&sizeFrameHeader,2); 

	unsigned short numLine=sizeFrameHeader/2;


	//lecture des offsets de ligne
	unsigned short *lineOffset = new unsigned short[numLine+1];
	input->read((char *)(lineOffset+1),2 * (numLine-1));

	lineOffset[0] = sizeFrameHeader;

	//bricolage pour le dernier offset

	if (frame < (int)header.frameHeader.size()-1)
		lineOffset[numLine] = header.frameHeader[frame+1].offset - fHeader.offset;// - sizeFrameHeader;
	else
	{
		std::streampos p = input->tellg();
		input->seekg(0,std::ios::end);

		lineOffset[numLine] = (unsigned int)((unsigned int)input->tellg()-streamPos0) - fHeader.offset;// - sizeFrameHeader;

		input->seekg((unsigned int)p-streamPos0,std::ios::beg);
	}

	off.start   = fHeader.offset;//input->tellg();
	off.numLine = numLine;
	off.offset  = lineOffset;
}










