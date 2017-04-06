#ifndef __RX_PALETTE
#define __RX_PALETTE
#include <vector>


class RXPalette
{
public:
	~RXPalette(void);

	union Color
	{
			struct 
			{
				unsigned char b,g,r,a;
			}component;

			unsigned int color;
	};


	const Color & operator[]  (int i) const{ return color[i];}
	const std::vector<Color> & getPalette() const  { return color;}

protected:
	RXPalette(void);
	void setColor(int i, const Color &col) { color[i] = col;}
	void setColor(int i, unsigned int col) { color[i].color = col;}

private:
	std::vector<Color> color;
};


#endif
