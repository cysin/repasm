#ifndef RX_MAP__H
#define RX_MAP__H

#include <string>
#include <misc/RXHashmap.h>
#include <deque>
#include <stdexcept>

class RXMapSection;
class RXMapERASection;
class RXMapDIMSection;
class RXMapUNITSection;
class RXMapTHG2Section;
class RXMapMTXMSection;
class RXMapVERSection;

class RXMapInvalidMapException : public std::runtime_error
{
public:
	RXMapInvalidMapException(const std::string &what);
};


class RXMap
{
public:
	virtual ~RXMap(void);



	const RXMapSection * getRawSection(const std::string &name) const;


	//Specialized sections
	const RXMapERASection  * getERA()  const;
	const RXMapDIMSection  * getDIM()  const;
	const RXMapUNITSection * getUNIT() const;
	const RXMapTHG2Section * getTHG2() const;
	const RXMapMTXMSection * getMTXM() const;
	const RXMapVERSection * getVER() const;


	//Quick access to standard value
	int getWidth() const;
	int getHeight()const;

    std::string getMapName() const;
    std::string getMapDescription() const;

	void checkMap() const;


protected:
	void addSection(const std::string &name, RXMapSection *section);
	RXMap(void);

private:


	typedef RXTPtrHash<std::string, RXMapSection> RXTSectionHash;
	typedef std::deque<RXMapSection *> RXTSectionList;

	RXTSectionHash sectionHash;
	RXTSectionList list;


	template <typename T>
	const T *getSection(const std::string &name) const
	{
		RXTSectionHash::const_iterator it = sectionHash.find(name);
		if (it != sectionHash.end())
			return (const T *)it->second;

		return NULL;
	}

	template <typename T>
	T *getSection(const std::string &name)
	{
		RXTSectionHash::iterator it = sectionHash.find(name);
		if (it != sectionHash.end())
			return (T *)it->second;

		return NULL;
	}

	void mergeMTXM(RXMapSection *section);


	void checkSectionExistance(const std::string &name) const;
};


#endif
