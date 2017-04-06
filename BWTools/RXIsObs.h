#ifndef __RX__IS__OBS
#define __RX__IS__OBS

class BWrepFile;
class BWrepPlayer;

class RXIsObs
{
public:
	RXIsObs(void);
	~RXIsObs(void);

	static bool IsObs(const BWrepFile &file, const BWrepPlayer &player);
};

#endif
