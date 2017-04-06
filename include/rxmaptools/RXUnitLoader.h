#ifndef RX_UNITLOADER
#define RX_UNITLOADER

#include <memory>
#include <misc/RXHashmap.h>
#include <rxmapbase/RXMapEnum.h>
#include <rxmaptools/file/RXDAT.h>
#include "RXRenderableUnit.h"
#include <rxmaptools/RXScriptOpcodeReader.h>

class RXFileManager;
class RXIScript;
class RXUnitDAT;
class RXImageDAT;
class RXLOLoader;
class RXUnitLayerImpl;

class RXUnitLoader
{
public:
	RXUnitLoader(RXFileManager *fm);
	~RXUnitLoader(void);


	RXRenderableUnit * getDoodads(int x, int y, int doodadsID);
	RXRenderableUnit * getUnit(int x, int y, RXMapEnum::UnitType unitID, int owner);

	RXRenderableUnit * getMinerals(int x, int y, RXMapEnum::UnitType unitID, int mineralAmount);
	RXRenderableUnit * getStartingLocation(int x, int y, RXMapEnum::UnitType unitID, int colorID);

private:
	typedef RXDAT<unsigned short>  RXTFlingyDat;
	typedef RXDAT<unsigned short> RXTSpriteDat;

	std::auto_ptr<RXUnitDAT>	units;
	std::auto_ptr<RXImageDAT>   images;
	std::auto_ptr<RXTFlingyDat> flingy;
	std::auto_ptr<RXTSpriteDat> sprite;
	std::auto_ptr<RXIScript>	script;
	std::auto_ptr<RXLOLoader>	loCache;

	


	typedef RXTPtrHash<RXMapEnum::UnitType, RXRenderableUnit> RXTRenderableUnitHash;
	RXTRenderableUnitHash loadedUnit;


	RXRenderableUnit::RXType unitToType(RXMapEnum::UnitType type);

	int getImageIndexFromImage(int unit);

	RXUnitLayerImpl * getLayerFromSpriteID(int unitID);
	RXUnitLayerImpl * getLayerFromUnitID(int unitID);
	RXUnitLayerImpl * getLayer(int imageID, int yOffset=0);

	class RXUnitLoaderEvent  : public RXScriptOpcodeReaderEvent
	{
	public:

		RXUnitLoaderEvent(RXUnitLoader *f, RXUnitLayerImpl *layer);
		virtual void OnActiveUnderlay(unsigned short imageID, unsigned char xOffset, unsigned char yOffset);
		virtual void OnActiveOverlay(unsigned short imageID, unsigned char xOffset, unsigned char yOffset);
		virtual void OnSetFrame(unsigned short imageNumber);
		virtual void OnUseParentFrame();

	private:
		RXUnitLayerImpl *layer;
		RXUnitLoader *fetch;
	};

	friend class RXUnitLoaderEvent; //this stupid line is here because gcc 2.95 doesn't seems to fully grasp the idea of nested class
	void fixUnit(RXUnitLayerImpl *impl, int uid);
};

#endif
