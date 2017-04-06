include ../config

all: librxmapbase.a


FILES.cpp =	RXCHKMap.cpp \
			RXMap.cpp \
			RXMapDIMSection.cpp \
			RXMapERASection.cpp\
			RXMapMTXMSection.cpp\
			RXMapSection.cpp	\
			RXMapSPRPSection.cpp\
			RXMapSTRSection.cpp\
			RXMapTHG2Section.cpp\
			RXMapUNITSection.cpp\
			RXMapVERSection.cpp\
			RXStartingLocation.cpp\
			RXMapExtractor.cpp\
			RXRepMap.cpp

FILES.o = $(FILES.cpp:.cpp=.o)

%.o: %.cpp
	$(CPP) $(CCFLAGS) -I$(HEADERS) -I$(LIBJPEGDIR) -I../StormLib -c $< -o $@
	
	
librxmapbase.a:	$(FILES.o)
	$(AR) cr ../libs/librxmapbase.a $(FILES.o)


clean:
	$(RM) *.o *.a
