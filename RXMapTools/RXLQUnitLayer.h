#ifndef __RX__LQUNITLAYER
#define __RX__LQUNITLAYER

#include <rxmaptools/RXUnitLayer.h>

class RXLQUnitLayer : public RXUnitLayer
{
public:
	RXLQUnitLayer(int width, int height, unsigned int color);
	~RXLQUnitLayer(void);

	virtual void render(RXUnitRenderer *to,int x,int y,int owner) const;



private:
	unsigned int color;
	int width, height;
};


#endif
