#ifndef RX_REPMAP__H
#define RX_REPMAP__H

#include "RXMap.h"


class BWrepFile;
class BWrepMapSection;

class RXRepMap : public RXMap
{
public:
	RXRepMap(const BWrepFile &file);
	~RXRepMap(void);

private:
	void addRXMapSection(const BWrepMapSection *section, const char *name);

	template <typename T>
	void addRepSection(const BWrepMapSection *section, const char *name);
};

#endif

