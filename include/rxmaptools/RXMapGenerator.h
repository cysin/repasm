#ifndef RX_MAPGENERATOR__H
#define RX_MAPGENERATOR__H

class RXTileset;
class RXRenderableMap;
class RXBitmap;

class RXMapGenerator
{
public:
	RXMapGenerator(void);
	~RXMapGenerator(void);

	//Generate the whole map at specified resolution
	RXBitmap *generate(const RXRenderableMap *map,const RXTileset *ts,int size, float &outReductionFactor);

	//Generate a subset of the map at full-resolution
	//tile means here subtile
	RXBitmap *generateSubset(const RXRenderableMap *map,const RXTileset *ts,int startTileX, int startTileY,int numTileX, int numTileY);

private:
	void computeMergeTile(const RXRenderableMap *map,const RXTileset *ts,RXBitmap *dest,int nbTile);	//quand on tombe pile poile sur des sous tiles

	//Generation taille reele
	void computeDirectCopy(const RXRenderableMap *rxmap,const RXTileset *ts,RXBitmap *destBitmap,
		int fromX, int fromY, int toX, int toY);	//quand on genere une image taille reele
	void computeGenericPath(const RXRenderableMap *rxmap,const RXTileset *ts,RXBitmap *dest,int dep); //marche tout le temps, mais c'est pas veloce
};

#endif
