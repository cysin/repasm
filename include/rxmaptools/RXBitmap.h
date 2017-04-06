#ifndef __RX_BITMAP
#define __RX_BITMAP

#include <fstream>
#include <string>


class RXPalette;

class RXBitmap
{
public:

	RXBitmap(void);
	RXBitmap(int width, int height, bool clear=false);
	RXBitmap(const unsigned char *index, int width, int height, const RXPalette &pal);
	RXBitmap(const RXBitmap *);

	~RXBitmap(void);

	void clear();


	//Minify bitmap by a fixed factor
	RXBitmap * minify(float reductionFactor, int numSubpixel);
	RXBitmap * minify(int newX, int newY, int numSubpixel);

	//Direct access to data
	const unsigned int *getPixel() const { return color;}
	unsigned int *getPixel() { return color;}

	//Dimensions info
	int getHeight() const { return height;}
	int getWidth() const  { return width;}

	//Dimensions info
	void saveToBMP(const std::string &destName) const;
	void saveToBMP(std::ostream &out)  const;

	void saveToJPEG(const std::string &destName, int compression) const;
	void saveToJPEG(std::ostream &out, int compression)  const;

	//Copy
	void copyInto(RXBitmap *dest, int x, int y)		const;
	void copyMaskTo(RXBitmap *dest, int x, int y)	const;
	void blendTo(RXBitmap *dest, int x, int y)		const;

	void fillRect(int x,int y, int width, int height, unsigned int color);

    void resetAlphaChannel();
private:

	//memory function
	void allocate(int width, int height, bool clear);
	void release();

	//bitmap object
	int width;
	int height;
	unsigned int *color;


	//Copy operation
	struct RXCopyRect
	{
		int destX, destY;

		int x, y;
		int width, height;
	};

	void getCopyRect(RXBitmap *dest, int x, int y, RXCopyRect &cp) const;
	void getFillRect(int x, int y, int rWidth, int rHeight, RXBitmap::RXCopyRect &cp) const;

	int getP(int numSub);
};



#endif











