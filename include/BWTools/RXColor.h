#ifndef __RX__COLOR
#define __RX__COLOR

#include <string>
#include <iostream>

class RXColor
{
public:
	~RXColor(void);
 
	enum ColorID
	{
		RED	=0,
		BLUE,
		TEAL,
		PURPLE,
		ORANGE,
		BROWN,
		WHITE,
		YELLOW,
		GREEN,
		PALEYELLOW,
		TAN,
		AQUA,
		PALEGREEN,
		BLUEISHGRAY,
		PALEYELLOW2,
		CYAN,
		BLACK,
		NEONBLUE,
		LAVENDER=21,
		BLACK2=22,
		SKYBLUE=30,
		PURPLE2=33
	};

	int getHTMLCode()				const {return colorCode;}			
	ColorID getColorID()				const {return colorID;}
	const std::string & getName()	const {return colorName;}

	void serialize(std::ostream &str);


protected:
	RXColor();

	std::string colorName;
	int colorCode;
	ColorID colorID;


};

#endif
