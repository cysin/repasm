#ifndef __RX_SPRP___SECTION
#define __RX_SPRP___SECTION

#include "RXMapSection.h"

class RXMapSPRPSection : public RXMapSection
{
public:
    RXMapSPRPSection(std::istream *str, int size);
    RXMapSPRPSection(const unsigned char *data, int size);
    ~RXMapSPRPSection(void);


    int getMapNameIndex() const;
    int getMapDescriptionIndex() const;

private:

};

#endif

