#ifndef __RX__WINNER
#define __RX__WINNER

#include "BWlib/BWrepAPI.h"
#include "BWTools/RXPlayer.h"
#include "BWTools/RXTeam.h"


class RXWinner
{
public:
	//RXWinner(RXPlayer *player1, RXPlayer *player2);
	RXWinner(const BWrepFile &repFile, const RXPlayerList &playersList);
	~RXWinner(void);

	void getWinner(int &winner, unsigned short &proba);

private:
	unsigned short probability; // from 1 = "pure guess" to 100 = "100% sure"
	int winnerNum;

	void computeWinner(const BWrepFile &repFile, const RXPlayerList &playersList);
	bool isObs(int playerSlot, const RXPlayerList &players);
};

#endif
