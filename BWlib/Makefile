include ../config

all: libbwrep.a

bwrepactions.o: 
	$(CPP) BWrepActions.cpp $(CCFLAGS) -I$(HEADERS) -o bwrepactions.o -c 

bwrepapi.o: 
	$(CPP) BWrepAPI.cpp $(CCFLAGS) -I$(HEADERS) -o  bwrepapi.o -c 

bwrepgamedata.o: 
	$(CPP) BWrepGameData.cpp $(CCFLAGS) -I$(HEADERS) -o bwrepgamedata.o -c

bwrepmap.o: 
	$(CPP) BWrepMap.cpp $(CCFLAGS) -I$(HEADERS)  -o bwrepmap.o -c
   
unpack.o:
	$(CPP) unpack.cpp $(CCFLAGS) -I$(HEADERS) -o unpack.o -c




libbwrep.a: unpack.o bwrepmap.o bwrepgamedata.o bwrepapi.o bwrepactions.o
	$(AR) cr ../libs/libbwrep.a bwrepmap.o bwrepgamedata.o unpack.o bwrepapi.o bwrepactions.o
 
 
install:
	@md $(HEADERS)
	$(CP) *.h $(HEADERS)
	
clean:
	$(RM) *.o *.a