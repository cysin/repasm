#ifndef RX_UNITLAYER__H
#define RX_UNITLAYER__H

#include <deque>
#include <rxmapbase/RXMapEnum.h>

class RXUnitLayer;
class RXUnitRenderer;



class RXUnitLayer
{
public:


	virtual ~RXUnitLayer(void);
	virtual void render(RXUnitRenderer *to,int x,int y,int owner) const = 0;



protected:
	RXUnitLayer(void);
};


#endif















