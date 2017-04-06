#ifndef RX_TILESET
#define RX_TILESET

#include <string>
#include <rxmapbase/RXMapEnum.h>
#include <vector>
#include <memory>


#include <rxmaptools/file/RXCV5.h>
#include <rxmaptools/file/RXVR4.h>
#include <rxmaptools/file/RXVX4.h>
#include <rxmaptools/file/RXWPE.h>

class RXFileManager;



class RXTileset
{
public:
	RXTileset(RXFileManager *fm, RXMapEnum::TileSet tileSet);
	~RXTileset(void);

	//Retrieves loaded tileset
	const RXVR4 * getVR4() const { return vr4.get();}
	const RXVX4 * getVX4() const { return vx4.get();}
	const RXCV5 * getCV5() const { return cv5.get();}

	const RXPalette * getPalette() const { return wpe.get();}


	//Static function helper
	static std::string nameFromTileset(RXMapEnum::TileSet tileSet);

private:

	void load(RXFileManager *fm, const std::string &path, const std::string &baseName);
	std::auto_ptr<RXVR4> vr4;
	std::auto_ptr<RXVX4> vx4;
	std::auto_ptr<RXCV5> cv5;
	std::auto_ptr<RXWPE> wpe;

	template <typename T, typename T2>
	void load(RXFileManager *fm,T2 &dest,const std::string &path, const std::string &baseName, const std::string &extension);
};


#endif

