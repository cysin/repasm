#ifndef RX_VR4
#define RX_VR4

#include <iostream>
#include <vector>


class RXVR4
{
public:
	RXVR4(std::istream *input);
	~RXVR4(void);

	struct Tile 
	{
		unsigned char val[8*8];
	};



	const std::vector<Tile> getTile() const { return tile;}
	const Tile & operator [] (int i)		{ return tile[i];}

private:

	std::vector<Tile> tile;

	void read(std::istream *input);


};

#endif
