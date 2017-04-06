include ../config

all: librxfile.a

FILES.cpp = RXFileManager.cpp RXFSFileManager.cpp RXMPQReader.cpp RXSCMpqReader.cpp

FILES.o = $(FILES.cpp:.cpp=.o)

%.o: %.cpp
	$(CPP) $(CCFLAGS) -I$(HEADERS) -I../StormLib -c $< -o $@
	
	

librxfile.a:	$(FILES.o)
	$(AR) cr ../libs/librxfile.a $(FILES.o)

	
clean:
	$(RM) *.o *.a