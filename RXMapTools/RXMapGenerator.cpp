#include "stdafx.h"
#include <rxmaptools/RXMapGenerator.h>
#include <rxmaptools/RXRenderableMap.h>
#include <rxmapbase/sections/RXMapMTXMSection.h>
#include <rxmaptools/RXBitmap.h>
#include <rxmaptools/RXTileset.h>

RXMapGenerator::RXMapGenerator(void)
{
}

RXMapGenerator::~RXMapGenerator(void)
{
}


RXBitmap *RXMapGenerator::generate(const RXRenderableMap *map,const RXTileset *ts,int size, float &outReductionFactor)
{


	int mapWidth   = map->getMapWidth() * 4;
	int mapHeigth  = map->getMapHeight()* 4;




	int bitmapWidth;
	int bitmapHeight;

	if (mapWidth>=mapHeigth)
	{
		if (size > map->getMapWidth()*32)
			size = map->getMapWidth()*32;

		bitmapWidth = size;
		bitmapHeight =  (bitmapWidth* mapHeigth) / mapWidth;
	}
	else
	{
		if (size > map->getMapHeight()*32)
			size = map->getMapHeight()*32;

		bitmapHeight = size;
		bitmapWidth = (bitmapHeight* mapWidth) / mapHeigth;
	}


	RXBitmap *dest = new RXBitmap(bitmapWidth, bitmapHeight, false);


	int dep = (mapWidth<<16)  / bitmapWidth; //same on height

	if (dep == 8192) 
	{
		//8192 means 1/8 of a subtile
		//one subtile is divided into 8x8pixels
		//So, 8192 means that one pixel in the destination bitmap = one pixel in the source
		computeDirectCopy(map, ts, dest,0,0,mapWidth, mapHeigth);
	}
	else if ((dep & 0xFFFF) < 7) //the last 3 bits won't change anything
	{
		//aligned on subtile boundaries
		computeMergeTile(map, ts, dest, dep>>16);
	}
	else
	{
		//generic codepath
		computeGenericPath(map,ts, dest, dep);
	}

	if ((mapWidth * 8) == bitmapWidth)
		outReductionFactor = 1.0f;
	else
		outReductionFactor = (float)(mapWidth * 8) / (float)bitmapWidth;




	return dest;
}








RXBitmap *RXMapGenerator::generateSubset(const RXRenderableMap *map,const RXTileset *ts,int startTileX, int startTileY,int numTileX, int numTileY)
{


	int mapWidth   = map->getMapWidth() * 4;
	int mapHeigth  = map->getMapHeight()* 4;

	if (startTileX >= mapWidth || startTileY>=mapHeigth)
		return NULL;

	if (startTileX<0)
    {
        numTileX+=startTileX; //startTileX is negative
        startTileX = 0;
    }

	if (startTileY<0)
    {
        numTileY+=startTileY; //startTileY is negative
        startTileY = 0;
    }
	int endX = startTileX + numTileX;
	int endY = startTileY + numTileY;

	if (endX > mapWidth)
		endX = mapWidth;

	if (endY > mapHeigth)
		endY = mapHeigth;

	if (startTileX == endX || startTileY == endY)
		return NULL;

	int bWidth  = (endX - startTileX) * 8;
	int bheight = (endY - startTileY) * 8;


	RXBitmap *res = new RXBitmap(bWidth, bheight, false);
	computeDirectCopy(map, ts, res, startTileX, startTileY,endX, endY);

	return res;
}






void RXMapGenerator::computeMergeTile(const RXRenderableMap *rxmap,const RXTileset *ts,RXBitmap *dest,int nbTile)
{
	const unsigned short *map = (const unsigned short *)rxmap->getMapTile();


	unsigned int *res = dest->getPixel();
	int sizeX = dest->getWidth();
	int sizeY = dest->getHeight();


	//Divisor = nombre de pixels utilisés dans la map originale pour calculer un px de notre minimap
	int divisor = nbTile * nbTile * 64; //64 = 8*8



	//deux trois trucs pour pas faire des appels de fonctions en tout sens, meme si fort risque d'inline
	
	const std::vector<int> &index					= ts->getCV5()->getIndex();
	const std::vector<RXVX4::SubTileIndex> &subTile = ts->getVX4()->getSubtile();
	const std::vector<RXPalette::Color> &palette	= ts->getPalette()->getPalette();
	const std::vector<RXVR4::Tile> &tile			= ts->getVR4()->getTile();





	int mapWidth = rxmap->getMapWidth();
	int tx=0, ty=0;


	for (int i=0;i<sizeY;i++)
	{
		for (int j=0;j<sizeX;j++)
		{
			//Accumulateur de composante
			unsigned int r=0,g=0,b=0;


			int ty2 = ty;

			for (int it=0;it<nbTile;it++)
			{
				int tx2 = tx;

				//me suis permis de remplacer les divs et les modulo par qqchose de moins gourmand
				//visiblement on gagne 10ms sur 70ms de calcul comme ca :tartragnan:

				int k  = ty2 >>2;
				int j1 = ty2&3; 

				const unsigned short *pMap = map + k*mapWidth;


				for (int jt=0;jt<nbTile;jt++)
				{
					int l  = tx2 >>2;
					int i1 = tx2 -(l<<2); 

					int cv5Idx = pMap[l];

					int vx4 = index[cv5Idx];// tl.getCV5Index(cv5Idx);
					int vr4 = subTile[vx4].index[i1][j1];//.getVX4Subtile(vx4, i1, j1);


					//On divise vr4 par 2 gaiement (histoire d'orientation de tile)
					const unsigned char *src = tile[vr4/2].val; 

					//on se balade dans la tile et on ajoute sur nos composantes
					//ca devrait pouvoir s'optimiser pour du MMX ca (:tartragnan:)

					for (int m=0;m<8*8;m++)
					{
						r+=palette[src[m]].component.r;
						g+=palette[src[m]].component.g;
						b+=palette[src[m]].component.b;
					}

					tx2++; //tile suivante
				}
				ty2++; //ligne de tile suivante
			}

			//Zou, on divise le bins
			r/=divisor;
			g/=divisor;
			b/=divisor;


			//on stocke le pixel calculé
			*res = (r<<16)|(g<<8)|b;
			res++;

			//et on continue
			tx+=nbTile;
		}

		tx=0;
		ty+=nbTile;
	}

}











void RXMapGenerator::computeGenericPath(const RXRenderableMap *rxmap,const RXTileset *ts,RXBitmap *dest,int dep)
{
	const unsigned short *map = (const unsigned short *)rxmap->getMapTile();


	unsigned int *res = dest->getPixel();
	int sizeX = dest->getWidth();
	int sizeY = dest->getHeight();


	//deux trois trucs pour pas faire des appels de fonctions en tout sens, meme si fort risque d'inline

	const std::vector<int> &index					= ts->getCV5()->getIndex();
	const std::vector<RXVX4::SubTileIndex> &subTile = ts->getVX4()->getSubtile();
	const std::vector<RXPalette::Color> &palette	= ts->getPalette()->getPalette();
	const std::vector<RXVR4::Tile> &tile			= ts->getVR4()->getTile();





	int mapWidth = rxmap->getMapWidth();
	int mapHeight = rxmap->getMapHeight();
	int tx=0, ty=0;


	for (int i=0;i<sizeY;i++)
	{
		for (int j=0;j<sizeX;j++)
		{
			//Accumulateur de composante
			unsigned int r=0,g=0,b=0;
			int count = 0;



			int ty2 = ty;

			int depY = dep;
			while(depY>0)
			{
				//Calcul de connerie
				int startY = (ty2&0xFFFF) >> 13;
				int endY;

				//calcul de endX
				int ty2dep = ty2+depY;
				int nextDepY;
				int nextTileY;

				if ((ty2&0xFFFF0000) == (ty2dep&0xFFFF0000))
				{
					//si on reste dans la même sous tile
					endY = (ty2dep&0xFFFF) >> 13;
					nextDepY = 0;
					nextTileY = ty2dep;
				}
				else
				{
					//changement de tile. on clampe
					nextTileY = ((ty2&0xFFFF0000)+(1<<16));
					nextDepY = ty2dep - nextTileY;
					endY = 8;
				}


				//me suis permis de remplacer les divs et les modulo par qqchose de moins gourmand
				//visiblement on gagne 10ms sur 70ms de calcul comme ca :tartragnan:

				int k  = (ty2>>16)>>2;
				if(k >= mapHeight)
					k = mapHeight - 1;

				int j1 = (ty2>>16)&0x3; 

				const unsigned short *pMap = map + k*mapWidth;

				int tx2 = tx;
				int depX = dep;

				while(depX>0)
				{
					int l  = (tx2>>16) >>2;
					int i1 = (tx2>>16)&0x3; 

					int cv5Idx = pMap[l];

					int vx4 = index[cv5Idx];
					int vr4 = subTile[vx4].index[i1][j1];





					int startX = (tx2&0xFFFF) >> 13;
					int endX;

					//calcul de endX
					int tx2dep = tx2+depX;
					int nextDepX;
					int nextTileX;

					if ((tx2&0xFFFF0000) == (tx2dep&0xFFFF0000))
					{
						//si on reste dans la même sous tile
						endX = (tx2dep&0xFFFF) >> 13;
						nextDepX = 0;
						nextTileX = tx2dep;
					}
					else
					{
						//changement de tile. on clampe
						nextTileX = ((tx2&0xFFFF0000)+(1<<16));
						nextDepX = tx2dep - nextTileX;
						endX = 8;
					}


					//On divise vr4 par 2 gaiement (histoire d'orientation de tile)
					const unsigned char *src = tile[vr4/2].val; 

					//on se balade dans la tile et on ajoute sur nos composantes
					//ca devrait pouvoir s'optimiser pour du MMX ca (:tartragnan:)

					if ((vr4&1)==0)
					{
						src +=startY*8;
						for (int ni=startY;ni<endY;ni++)
						{
							for (int nj=startX;nj<endX;nj++)
							{
								r+=palette[src[nj]].component.r;
								g+=palette[src[nj]].component.g;
								b+=palette[src[nj]].component.b;

								count++;
							}
							src+=8;
						}
					}
					else
					{
						src +=8-startX-1 + startY*8;
						for (int ni=startY;ni<endY;ni++)
						{
							for (int nj=0;nj<endX-startX;nj++)
							{
								r+=palette[src[-nj]].component.r;
								g+=palette[src[-nj]].component.g;
								b+=palette[src[-nj]].component.b;

								count++;
							}
							src+=8;
						}
					}


					/*
					for (int m=0;m<8*8;m++)
					{
						r+=palette[src[m]].r;
						g+=palette[src[m]].g;
						b+=palette[src[m]].b;
					}
					*/

					tx2 = nextTileX; //tile suivante
					depX = nextDepX;
				}
				ty2=nextTileY; //ligne de tile suivante
				depY = nextDepY;
			}

			//Zou, on divise le bins
			r/=count;
			g/=count;
			b/=count;


			//on stocke le pixel calculé
			*res = (r<<16)|(g<<8)|b;
			res++;

			//et on continue
			tx+=dep;
		}

		tx=0;
		ty+=dep;
	}

}











void RXMapGenerator::computeDirectCopy(const RXRenderableMap *rxmap,const RXTileset *ts,RXBitmap *destBitmap,
									   int fromX, int fromY, int toX, int toY)
{

	const unsigned short *map = (const unsigned short *)rxmap->getMapTile();


	unsigned int *dest = destBitmap->getPixel();
	int stride = destBitmap->getWidth();

	int width  = rxmap->getMapWidth();
	int height = rxmap->getMapHeight();


	//deux trois trucs pour pas faire des appels de fonctions en tout sens, meme si fort risque d'inline
	const std::vector<int> &index					= ts->getCV5()->getIndex();
	const std::vector<RXVX4::SubTileIndex> &subTile = ts->getVX4()->getSubtile();
	const std::vector<RXPalette::Color> &palette	= ts->getPalette()->getPalette();
	const std::vector<RXVR4::Tile> &tile			= ts->getVR4()->getTile();


	for (int i=fromY;i<toY;i++)
	{
		int k  = i>>2;
		int j1 = i&3; 

		const unsigned short *pMap = map + k*width;
		unsigned int *dest2 = dest;

		for (int j=fromX;j<toX;j++)
		{
			int l  = j >>2;
			int i1 = j&3; 

			int cv5Idx = pMap[l];

			int vx4 = index[cv5Idx];
			int vr4 = subTile[vx4].index[i1][j1];

			const unsigned char *src = tile[vr4/2].val; 
			unsigned int *dest3 = dest2;

			if ((vr4&1)==0)
			{
				for (int ni=0;ni<8;ni++)
				{
					for (int nj=0;nj<8;nj++)
					{
						dest3[nj] = palette[*src].color;
						src++;
					}
					dest3+=stride;
				}
			}
			else
			{
				for (int ni=0;ni<8;ni++)
				{
					for (int nj=0;nj<8;nj++)
					{
						dest3[nj] = palette[src[7-nj]].color;
					}
					src+=8;
					dest3+=stride;
				}
			}

			dest2+=8;
		}


		dest+=8*stride;
	}
}











