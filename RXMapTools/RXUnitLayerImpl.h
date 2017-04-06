#ifndef RX_UNITLAYERIMPL
#define RX_UNITLAYERIMPL

#include <rxmaptools/RXUnitLayer.h>



class RXUnitLayerImpl : public RXUnitLayer
{
public:
	RXUnitLayerImpl(int imageID, RXMapEnum::Palette palette, int yOffset = 0);
	virtual ~RXUnitLayerImpl(void);

	void addOverlay (RXUnitLayerImpl *overlay);
	void addUnderlay(RXUnitLayerImpl *underlay);

	
	void setActiveFrame		 (int fc);
	void setFollowParentFrame();

	void setOffset(int x, int y);

	virtual void render(RXUnitRenderer *to,int x,int y,int owner) const;



protected:
	typedef std::deque<RXUnitLayerImpl *> RXTUnitLayerImplList;

	RXUnitLayerImpl(void);

	RXMapEnum::Palette palette;

	int imageID;
	int frameNumber;
	int yOffset;

	int overlayX;
	int overlayY;

	RXTUnitLayerImplList overlay;
	RXTUnitLayerImplList underlay;

private:
	void pRender(RXUnitRenderer *to,int x,int y, int owner, const RXUnitLayerImpl *parent) const;
};

#endif
