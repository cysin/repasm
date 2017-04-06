#ifndef RX_BITMAPCACHE
#define RX_BITMAPCACHE

#include <memory>
#include <misc/RXHashmap.h>
#include <rxmapbase/RXMapEnum.h>


class RXPalette;
class RXBitmap;
class RXGRPLoader;
class RXImageDAT;

class RXFileManager;



class RXBitmapCache
{
public:
	RXBitmapCache(RXFileManager *fm);
	~RXBitmapCache(void);


	void setActivePalette(const RXPalette *palette) {this->activePalette = palette;}
	const RXBitmap * getUnitBitmap(int imageID, int frame, int owner, float reductionFactor);




private:


	const RXPalette *activePalette;
	std::auto_ptr<RXGRPLoader> grpLoader;
	std::auto_ptr<RXImageDAT> image;
	RXFileManager *ressource;


	template <typename T>
	struct RXTFloatKey
	{
		float ratio;

		union
		{
			T value;
			size_t key;
		};

		operator size_t() const 
		{
			//well..... i suppose there's a better to do, but I feel a bit lazy, right now
			const int * pRatio = (const int *)&ratio;
			return (*pRatio)^key;
		}
	};


	struct RXNeutralKeyStruct
	{
		unsigned short imageID;
		unsigned short frameNumber;
	};

	struct RXNonNeutralKeyStruct
	{
		unsigned short imageID;
		unsigned char  teamID;
		unsigned short frameNumber;
	};


	typedef RXTFloatKey<RXNeutralKeyStruct> RXTNeutralImageKey;
	typedef RXTPtrHash<RXTNeutralImageKey, RXBitmap>  RXTNeutralHash;


	typedef RXTFloatKey<RXNonNeutralKeyStruct> RXTNonNeutralImageKey;
	typedef RXTPtrHash<RXTNonNeutralImageKey, RXBitmap>  RXTNonNeutralHash;

	

	RXTNeutralHash		neutralHash;
	RXTNonNeutralHash	nonNeutralHash;


	const RXBitmap *getNeutralImage(int imageID, int frame, RXMapEnum::Palette palette, float reductionFactor);
	const RXBitmap *getNonNeutralImage(int imageID, int frame, int owner,  float reductionFactor);
	

	template<typename Key>
	const RXBitmap *getOrCreateImage(RXTPtrHash<Key, RXBitmap> &cache, Key key, float reductionFactor, const RXPalette *p);



	//Colored unit
	int getResizeQuality(float reductionFactor);



};

#endif


