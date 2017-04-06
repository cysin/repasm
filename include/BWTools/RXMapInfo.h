#include <string>
#include <iostream>
#include <vector>

#include <rxmapbase/RXMap.h>
#include <rxmapbase/RXMapEnum.h>

class BWrepFile;
class RXMap;


class RXMapInfo
{
public:

	RXMapInfo(const RXMap *file);
	RXMapInfo(std::istream &stream);
	~RXMapInfo();

	const std::string & getName()			const {return mapName;}
    const std::string & getDescription()	const {return mapDesc;}

	const std::string & getCleanName()		const {return cleanedMapName;}
	const std::string & getDimensions()		const {return dimensions;}

	int                getWidth()			const {return width;}
	int                getHeight()			const {return height;}
    RXMapEnum::TileSet getTileSet()         const {return tileset;}

	const std::string & getTileSetName()	const {return tilesetName;}
	bool				isBroodMap()				const {return isBW;}
	const std::vector<int> & getStartLocs()		const {return startLocs;}

    void serialize(std::ostream &str) const;

private:
	std::string mapName;
    std::string mapDesc;
	std::string cleanedMapName;
	std::string dimensions;
	std::string tilesetName;
	std::vector<int> startLocs;


	int width;
	int height;
    RXMapEnum::TileSet tileset;
	bool isBW;

    void buildFormatedInfo();
	void cleanMapName();
	void buildDimensions();
	void buildTileset(const RXMap *file);
	void buildTilesetName();
};
