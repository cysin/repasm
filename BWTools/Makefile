include ../config

all: libbwtools.a

RXAPM.o: 
	$(CPP) RXAPM.cpp $(CCFLAGS) -I$(HEADERS)  -c 

RXIsObs.o: 
	$(CPP) RXIsObs.cpp $(CCFLAGS) -I$(HEADERS)  -c 

RXColor.o: 
	$(CPP) RXColor.cpp $(CCFLAGS) -I$(HEADERS)  -c 

RXColorImpl.o: 
	$(CPP) RXColorImpl.cpp $(CCFLAGS) -I$(HEADERS)  -c 


RXPlayer.o: 
	$(CPP) RXPlayer.cpp $(CCFLAGS) -I$(HEADERS)  -c 

RXPlayerImpl.o: 
	$(CPP) RXPlayerImpl.cpp $(CCFLAGS) -I$(HEADERS)  -c 

RXPlayerImport.o: 
	$(CPP) RXPlayerImport.cpp $(CCFLAGS) -I$(HEADERS)  -c 

RXRace.o: 
	$(CPP) RXRace.cpp $(CCFLAGS) -I$(HEADERS)  -c 

RXWinner.o: 
	$(CPP) RXWinner.cpp $(CCFLAGS) -I$(HEADERS)  -c 


RXTeamManager.o: 
	$(CPP) RXTeamManager.cpp $(CCFLAGS) -I$(HEADERS)  -c 

RXTwoPlayer.o: 
	$(CPP) RXTwoPlayer.cpp $(CCFLAGS) -I$(HEADERS)  -c 

RXVersionFromDate.o:
	$(CPP) RXVersionFromDate.cpp $(CCFLAGS) -I$(HEADERS)  -c 

RXVersion.o:
	$(CPP) RXVersion.cpp $(CCFLAGS) -I$(HEADERS)  -c 
	
RXReplay.o: 
	$(CPP) RXReplay.cpp $(CCFLAGS) -I$(HEADERS)  -c 

RXActionCompiler.o: 
	$(CPP) RXActionCompiler.cpp $(CCFLAGS) -I$(HEADERS)  -c 

RXMatchup.o: 
	$(CPP) RXMatchup.cpp $(CCFLAGS) -I$(HEADERS)  -c 

RXMapInfo.o: 
	$(CPP) RXMapInfo.cpp $(CCFLAGS) -I$(HEADERS)  -c 
	
libbwtools.a: RXMapInfo.o RXVersion.o RXVersionFromDate.o RXPlayerImport.o RXIsObs.o RXReplay.o RXTwoPlayer.o RXTeamManager.o RXWinner.o RXRace.o RXPlayerImpl.o RXPlayer.o RXColorImpl.o RXColor.o RXAPM.o RXActionCompiler.o RXMatchup.o
	$(AR) cr ../libs/libbwtools.a RXMapInfo.o RXVersion.o RXVersionFromDate.o RXPlayerImport.o RXIsObs.o RXReplay.o  RXTwoPlayer.o RXTeamManager.o RXWinner.o RXRace.o RXPlayerImpl.o RXPlayer.o RXColorImpl.o RXColor.o RXAPM.o RXActionCompiler.o RXMatchup.o
 
install:
	@md $(HEADERS)
	$(CP) *.h $(HEADERS)
	
clean:
	$(RM) *.o *.a
