#ifndef RX_UNITDAT__H
#define RX_UNITDAT__H

#include <iostream>

class RXUnitDAT
{
public:
	RXUnitDAT(std::istream *str);
	~RXUnitDAT(void);


	int getImage(int unitID)	 const { return graphics[unitID];}
	int getAnimLevel(int unitID) const { return animationLevel[unitID];}

	bool hasSubunit(int unitID) const	{ return subunit1[unitID] != 228;}
	int  getSubunit(int unitID) const		{ return subunit1[unitID];}


private:

	void read(std::istream *input);

	template <typename T>
	void readArray(std::istream *input,T *&dest, int num);

	unsigned char *graphics;
	unsigned short *subunit1;
	unsigned char *animationLevel;

};

#endif
