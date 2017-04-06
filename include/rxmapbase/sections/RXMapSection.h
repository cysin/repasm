#ifndef __RX_MAPSECTION
#define __RX_MAPSECTION

#include <iostream>
#include <string>

class RXMapSection
{
public:
	RXMapSection(const std::string &name,std::istream *str, int size);
	RXMapSection(const std::string &name,const unsigned char *data, int size);
	virtual ~RXMapSection(void);

	const std::string & getName() const { return name;}

	unsigned char *getRawData() { return pRawData;}
	const unsigned char *getRawData() const	{ return pRawData;}
	int	getSectionSize() const		{ return sectionSize;}






protected:

	template <typename TSIZE, typename RET>
	const RET getVal(int offset = 0)
	{
		TSIZE * ptr = (TSIZE *)(pRawData + offset);
		return (RET)(*ptr);
	}

	template <typename TSIZE, typename RET>
	void getValUpdateOffset(RET & val,int &offset)
	{
		TSIZE * ptr = (TSIZE *)(pRawData + offset);
		val =  (RET)(*ptr);
		offset+=sizeof(TSIZE);
	}

	template <typename TSIZE, typename RET>
	const void setVal(RET & val,int offset = 0)
	{
		TSIZE * ptr = (TSIZE *)(pRawData + offset);
		val =  (RET)(*ptr);
	}

private:
	const std::string name;
	unsigned char *pRawData;
	int   sectionSize;


};

#endif

