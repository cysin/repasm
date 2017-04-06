#include "stdafx.h"
#include <rxmaptools/RXPalette.h>
#include <rxmaptools/RXBitmap.h>
#include <stdexcept>

RXBitmap::RXBitmap(void)
{
	color = NULL;
	width = height = 0;
}

RXBitmap::~RXBitmap(void)
{
	release();
}


RXBitmap::RXBitmap(int pWidth, int pHeight, bool clear)
{
	color = NULL;
	allocate(pWidth, pHeight, clear);
}


RXBitmap::RXBitmap(const unsigned char *index, int pWidth, int pHeight, const RXPalette &pal)
{
	color = NULL;
	allocate(pWidth, pHeight, false);

	const std::vector<RXPalette::Color> &vec = pal.getPalette();
	const RXPalette::Color * palette = &vec[0];

	for (int i=0;i<pWidth * pHeight;i++)
		color[i] = palette[index[i]].color;
}



RXBitmap::RXBitmap(const RXBitmap *btm)
{
	if (btm->color == NULL)
	{
		color = NULL;
		width = height = 0;
	}
	else
	{
		allocate(btm->width, btm->height, false);
		memcpy(color, btm->color, width * height * 4);
	}
}


void RXBitmap::resetAlphaChannel()
{
    for (int i=0;i<width * height;i++)
        color[i] |= 0xFF000000;
}




RXBitmap * RXBitmap::minify(float factor, int numSubpixel)
{
	if (factor < 1.0f)
		return NULL;
	if (factor == 1.0f)
		return new RXBitmap(this);
	

	int newX = (int)((float)width  / factor);
	int newY = (int)((float)height / factor);

	return minify(newX, newY, numSubpixel);
}


RXBitmap * RXBitmap::minify(int newX, int newY, int numSubpixel)
{
	if(!color)
		return NULL;

	if (newX >= width || newY >= height)
		return NULL;

	if (newX<=0 || newY <=0)
		return NULL;

	int dec = getP(numSubpixel);

	if (dec <= 0)
		return NULL;

	int width2  = width<<dec;
	int height2 = height<<dec;

	int widthInc  = width2 / newX;
	int heightInc = height2 / newY;



	int yPos = 0;
	int xPos = 0;

	unsigned int *src = color;

	RXBitmap *res = new RXBitmap(newX, newY, false);

	unsigned int *dst = res->getPixel();

	int surface = ((widthInc>>dec))*((heightInc>>dec));
	surface *= (numSubpixel * numSubpixel);



	for (int i=0;i<newY;i++)
	{
		for (int j=0;j<newX;j++)
		{

			unsigned int a=0,r=0,g=0,b=0;

			int count = 0;
			for (int k=0;k<heightInc;k++)
			{
				unsigned int *src2 = src + ((yPos+k)>>dec)*width;

				unsigned char *src4 = (unsigned char *)src2;

				for (int l=0;l<widthInc;l++)
				{
					int tmpPos = (xPos+l) >> dec;
					tmpPos<<=2;

					a+= (int)(src4[tmpPos+3]);
					r+= (int)(src4[tmpPos+2]);
					g+= (int)(src4[tmpPos+1]);
					b+= (int)(src4[tmpPos]);

					count++;
				}
			}

			a/=count;
			r/=count;
			g/=count;
			b/=count;

			
			dst[j]=(a<<24)|(r<<16)|(g<<8)|b;
			xPos += widthInc;
		}

		dst+=newX;
		yPos+=heightInc;
		xPos = 0;
	}

	return res;

}









void RXBitmap::release()
{
	if (color)
		delete []color;

	width = height = 0;
}



void RXBitmap::allocate(int pWidth, int pHeight, bool clear)
{
	release();

	this->height = pHeight;
	this->width  = pWidth;

	color = new unsigned int [width * height];

	if (clear)
		memset(color, 0, width * height * 4);
}




int RXBitmap::getP(int numSub)
{

	int p = -1;
	for (int i=0;i<31;i++)
	{
		if (numSub & (1<<i))
		{
			if (p!=-1)
				return -1;
			p = i;
		}
	}

	return p;
}


//=========================================================//
// Copy to / Blend to
//=========================================================//


void RXBitmap::copyInto(RXBitmap *dest, int x, int y) const
{
	RXCopyRect rc;

	getCopyRect(dest, x, y, rc);


	if (rc.width <= 0)
		return;

	unsigned int * dst = dest->getPixel() + rc.destX + rc.destY * dest->getWidth();
	const unsigned int * src = getPixel() + rc.x + rc.y * width;


	for (int i=0;i<rc.height;i++)
	{
		memcpy(dst,src,rc.width* 4);

		src+=width;
		dst+=dest->getWidth();
	}

}





void RXBitmap::copyMaskTo(RXBitmap *dest, int x, int y) const
{
	RXCopyRect rc;

	getCopyRect(dest, x, y, rc);

	if (rc.width <= 0)
		return;

	if (rc.height <= 0)
		return;

	unsigned int * dst = dest->getPixel() + rc.destX + rc.destY * dest->getWidth();
	const unsigned int * src = getPixel() + rc.x + rc.y * width;


	for (int i=0;i<rc.height;i++)
	{

		//A deux balles, on fera mieux plus tard
		for (int j=0;j<rc.width;j++)
		{
			if (src[j]&0xFF000000)
				dst[j] = src[j];
		}

		src+=width;
		dst+=dest->getWidth();
	}
}


void RXBitmap::fillRect(int x,int y, int width, int height, unsigned int color)
{
	RXCopyRect rc;

	getFillRect(x, y, width, height, rc);

	if (rc.width <= 0)
		return;

	if (rc.height <= 0)
		return;

	unsigned int * dst = getPixel() + rc.x + rc.y * getWidth();

	for (int i=0;i<rc.height;i++)
	{
		for (int j=0;j<rc.width;j++)
			dst[j] = color;

		dst+=getWidth();
	}
}



#ifdef _MSC_VER
template<int I>
static __forceinline int alphaB(int src, int dst, int ab)
#else
template<int I>
static inline int alphaB(int src, int dst, int ab)
#endif
{
	
	int src2  = (src&I);
	int dst2 = (dst&I);

	return (src2 + (((dst2 -src2) * ab)>>8))&I;
}


void RXBitmap::blendTo(RXBitmap *dest, int x, int y) const
{
	RXCopyRect rc;

	getCopyRect(dest, x, y, rc);

	if (rc.width <= 0)
		return;


	unsigned int * dst = dest->getPixel() + rc.destX + rc.destY * dest->getWidth();
	const unsigned int * src = getPixel() + rc.x + rc.y * width;


	for (int i=0;i<rc.height;i++)
	{

		for (int j=0;j<rc.width;j++)
		{
			unsigned int factor = src[j]&0xFF000000;
			factor = (factor>>24);

			if (factor>5)
			{
				if (factor > 250 )
					dst[j] = src[j];
				else
				{
					factor = 255 - factor;
					//full fledged alpha blending
					int r = alphaB<0xFF0000>(src[j],dst[j],factor);
					int g = alphaB<0xFF00>	(src[j],dst[j],factor);
					int b = alphaB<0xFF>	(src[j],dst[j],factor);

					dst[j] = r | g | b;
				}
			}
		}

		src+=width;
		dst+=dest->getWidth();
	}
}




//=========================================================//
//===================   Copy operation  ===================//
//=========================================================//

void RXBitmap::getCopyRect(RXBitmap *dest, int x, int y, RXBitmap::RXCopyRect &cp) const
{

	//compute starting point
	if (x<0)
	{
		cp.destX = 0;
		cp.x = abs(x);
	}
	else
	{
		cp.x = 0;
		cp.destX = x;
	}

	if (y<0)
	{
		cp.destY = 0;
		cp.y = abs(y);
	}
	else
	{
		cp.y = 0;
		cp.destY = y;
	}

	int toCopyX = width - cp.x;
	int toCopyY = height - cp.y;


	if (cp.destX + toCopyX > dest->getWidth())
		cp.width = dest->getWidth() - cp.destX;
	else
		cp.width = toCopyX;

	if (cp.destY + toCopyY > dest->getHeight())
		cp.height = dest->getHeight() - cp.destY;
	else
		cp.height = toCopyY;
}




void RXBitmap::getFillRect(int x, int y, int rWidth, int rHeight, RXBitmap::RXCopyRect &cp) const
{

	//compute starting point
	if (x<0)
	{
		cp.x = 0;
		rWidth -= abs(x);
	}
	else
	{
		cp.x = x;
	}

	if (y<0)
	{
		cp.y = 0;
		rHeight -= abs(y);
	}
	else
	{
		cp.y = y;
	}


	if (cp.x + rWidth > width)
		cp.width = width - cp.x;
	else
		cp.width = rWidth;

	if (cp.y + rHeight > height)
		cp.height = height - cp.y;
	else
		cp.height = rHeight;
}











//=========================================================//
//=================== Writing operation ===================//
//=========================================================//

//BMP Header
struct BITMAPINFOHEADER{
	unsigned int		 biSize;
	int					biWidth;
	int					biHeight;
	short				biPlanes;
	short				biBitCount;
	unsigned int		biCompression;
	unsigned int		biSizeImage;
	int					biXPelsPerMeter;
	int					biYPelsPerMeter;
	unsigned int		biClrUsed;
	unsigned int		biClrImportant;
};



void RXBitmap::saveToBMP(const std::string &dest)  const
{
	std::ofstream str(dest.c_str(),std::ios::out | std::ios::binary | std::ios::trunc);
	if (str.is_open()==false)
		throw std::runtime_error("Could not save to  : " + dest);

	saveToBMP(str);

	str.close();
}




/*! \brief save the pic in BMP format to a flow
*  \param name : flow where to store the BMP
*  \return 0 if OK, -1 otherwise
*/

void RXBitmap::saveToBMP(std::ostream &out)  const
{


	unsigned int cInt = 0;
	unsigned short cShort = 0;


	out.write("BM",2);
	unsigned int size = width*height*3 + sizeof(BITMAPINFOHEADER);


	out.write((const char *)&size,4);          //taille du fichier (?)
	out.write((const char *)&cInt,4);          //reserved


	size = 54;
	out.write((const char *)&size,4);          //ou commence le bitmap (??)

	size=sizeof(BITMAPINFOHEADER);

	out.write((const char *)&size,4);		 //taille du header
	out.write((const char *)&width,4);		 //largeur
	out.write((const char *)&height,4);		 //hauteur

	cShort = 1;

	out.write((const char *)&cShort,2);			//nombre de plan

	cShort = 24;                    //bpp;
	out.write((const char *)&cShort,2);

	cInt=0;
	out.write((const char *)&cInt,4);			  //pas de compression (BI_RGB == 0)


	size = width*height*3;

	out.write((const char *)&size,4);
	cInt=0;
	out.write((const char *)&cInt,4);        //HResolution
	out.write((const char *)&cInt,4);        //VResolution
	out.write((const char *)&cInt,4);        //nbColors
	out.write((const char *)&cInt,4);        //nbImportant colors


	unsigned char align[3];


	int decal = 0;
	while(((width*3) + decal) %4)
		decal++;


	for (int i=height-1;i>=0;i--)
	{
		int pos = i * width;
		for (int j=0;j<width;j++)
		{
			//write RGB
			out.write((const char *)&color[pos+j],3);
		}

		//Align
		if (decal)
			out.write((const char *)align,decal);
	}

	if (out.fail())
		throw std::runtime_error("error while writing to bmp");
}

