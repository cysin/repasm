#ifndef __RX_STR___SECTION
#define __RX_STR___SECTION

#include "RXMapSection.h"

class RXMapSTRSection : public RXMapSection
{
public:
    RXMapSTRSection(std::istream *str, int size);
    RXMapSTRSection(const unsigned char *data, int size);
    ~RXMapSTRSection(void);



    const char * getString(int index) const;

private:

};

#endif
