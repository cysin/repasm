#ifndef RX_VX4
#define RX_VX4

#include <iostream>
#include <vector>

class RXVX4
{
public:
	RXVX4(std::istream *input);
	~RXVX4(void);

	struct SubTileIndex
	{
		int index[4][4];
	};


	const std::vector<SubTileIndex> & getSubtile() const{ return subTile;}
	const SubTileIndex & operator [] (int i) { return subTile[i];}


private:

	void read(std::istream *input);
	std::vector<SubTileIndex>	subTile;
};

#endif 
