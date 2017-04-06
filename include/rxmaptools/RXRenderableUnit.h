#ifndef RX_RENDERABLEUNIT__H
#define RX_RENDERABLEUNIT__H

#include <iostream> 

class RXUnitLayer;
class RXUnitRenderer;


class RXRenderableUnit
{
public:
	
	enum RXType
	{
		DOODADS				= 1,
		RESOURCE			= 2,
		STARTING_LOCATION	=4,
		BUILDING_UNIT		=8
	};

	
	RXRenderableUnit(int x, int y, int animationLevel, RXType type, RXUnitLayer *main, int owner,RXUnitLayer *sub = NULL);
	RXRenderableUnit(const RXRenderableUnit *unit);

	~RXRenderableUnit(void);





	int		getY()				    const { return y;}
	int		getAnimationLevel()		const { return animationLevel;}
	RXType	getType()				const { return type;}

	void render(RXUnitRenderer *target) const;



protected:

private:
	int		x,y;
	int		animationLevel;
	int		owner;
	RXType	type;

	RXUnitLayer *main, *sub1;



};





#endif




















