#ifndef RX_UNITRENDERER
#define RX_UNITRENDERER

#include <rxmapbase/RXMapEnum.h>

class RXUnitRenderer
{
public:

	virtual void blendBitmap(int tblID, int frameNumber, RXMapEnum::Palette palette, int owner, int centerX, int centerY)=0;
	virtual void fillRect(int centerX, int centerY, int width, int height, unsigned int color)=0;

};


#endif

