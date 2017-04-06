#ifndef RX_DAT
#define RX_DAT

#include <iostream>
#include <vector>

template<typename T>
class RXDAT
{
public:
	RXDAT(std::istream *str, int count)
	{
		read(str, count);
	}
	~RXDAT(void){}



	T operator[] (int i) { return dat[i];}

	int count() const { return (int) dat.size();}

private:

	std::vector<T> dat;

	void read(std::istream *str,int count)
	{
		dat.resize(count);

		str->read((char *)&dat[0],count * sizeof(T));
	}
};

#endif 
