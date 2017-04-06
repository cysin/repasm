#ifndef RX_CV5
#define RX_CV5

#include <iostream>
#include <vector>

class RXCV5
{
public:
	RXCV5(std::istream *);
	~RXCV5(void);


	//Access to cv5
	const std::vector<int> getIndex() const { return index;}

	//Shortcut
	int operator[] (int i) const { return index[i];}

private:
	void read(std::istream *input);

	std::vector<int> index;
};

#endif
