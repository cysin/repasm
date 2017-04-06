#ifndef __RX__ACTION__COMPILER
#define __RX__ACTION__COMPILER

#include <vector>

#include "BWlib/BWrepAPI.h"
#include "BWlib/BWrepActions.h"
class RXPlayerImpl;


typedef std::vector<BWrepAction> RXActionsList;
typedef std::vector<RXActionsList> RXRangedActionsList;


class RXActionCompiler
{
public:
	enum ActionType
	{
		ALL = 0,
		BUILD = 1,
		HOTKEY = 2,
		RESEARCH = 4,
		UPGRADE = 8
	};

	RXActionCompiler(const BWrepFile &rep, int playerSlot);
	RXActionCompiler(const BWrepFile &rep, const char *playerName);
	~RXActionCompiler(void);

	static void getLastActions(const BWrepFile &rep, RXActionsList &list, unsigned short numberOfActions = 20);

	RXActionsList getActionsForRange(unsigned long startFrame = 0, unsigned long stopFrame = 0, int acType = ALL);
	RXRangedActionsList getActionsAtFrequency(int frequencyInSecs = 10, int acType = ALL);
	std::vector<int> getAPMWithFrequency(int frequency = 10);
	int isPlayerFound() {return 1 ? player != NULL : 0;};

private:
	void getActions(const BWrepFile &rep);
	bool selectAction(const BWrepAction &action, int acType);
	
	std::vector<BWrepAction> actions;
	RXPlayerImpl* player;
	long duration;
};


#endif
