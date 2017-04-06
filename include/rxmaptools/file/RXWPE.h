#ifndef RX_WPE
#define RX_WPE


#include <iostream>
#include <rxmaptools/RXPalette.h>

class RXWPE : public RXPalette
{
public:
	RXWPE(std::istream *input);
	~RXWPE(void);

private:
	void read(std::istream *input);
};



#endif 
