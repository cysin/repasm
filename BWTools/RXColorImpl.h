#ifndef __RX_COLOR__IMPL
#define __RX_COLOR__IMPL

#include <iostream>
#include "BWTools/RXColor.h"

class BWrepFile;
class BWrepPlayer;

class RXColorImpl : public RXColor
{
public:
	RXColorImpl(const BWrepFile &file, const BWrepPlayer &player);
	RXColorImpl(std::istream &str);

	~RXColorImpl(void);


private:

	void		 buildWithID(ColorID id);
	ColorID		 getColorFromCode(const BWrepFile &file, const BWrepPlayer &player);
	const char * getColorNameFromCode(ColorID value);
	int			 getHTMLColorFromCode(ColorID colorIdx);
};

#endif
