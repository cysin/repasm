#ifndef RX_LO__H
#define RX_LO__H

#include <iostream>

class RXLO
{
public:
	RXLO(std::istream *str);
	~RXLO(void);


#pragma pack(1)
	struct RXOverlayOffset
	{
		char x;
		char y;
	};
#pragma pack()

	int getFrameCount()			const { return frameCount;}
	int getOverlayPerFrame()	const { return overlayPerFrame;}
	const RXOverlayOffset * getOffset(int frame, int overlay) const;

private:

	int frameCount;
	int overlayPerFrame;
	RXOverlayOffset * data;
};

#endif
