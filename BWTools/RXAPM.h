#ifndef __RX_APM
#define __RX_APM

#include <vector>

class BWrepPlayer;
class BWrepFile;

//Small APM Computation
class RXAPM
{
public:

    static int compute(const BWrepFile &file, const BWrepPlayer &player);

};

#endif
