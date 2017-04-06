include ../config

all: repasm.so

repasm.o:
	$(CPP) repasm.cpp $(CCFLAGS)  -I$(PHPDIR)/TSRM -I$(PHPDIR) -I$(PHPDIR)/main -I$(PHPDIR)/Zend -I$(PHPDIR) -I$(HEADERS) -I../StormLib $(PHPFLAGS) -O2 -o repasm.o -c

repasmmap.o:
	$(CPP) repasmmap.cpp $(CCFLAGS)  -I$(PHPDIR)/TSRM -I$(PHPDIR) -I$(PHPDIR)/main -I$(PHPDIR)/Zend -I$(PHPDIR) -I$(HEADERS) -I../StormLib $(PHPFLAGS) -O2 -o repasmmap.o -c

repasm.so : repasm.o repasmmap.o
		g++ -shared -rdynamic -L/usr/local/lib -L$(LIB) -o ../libs/repasm.so repasmmap.o repasm.o -lstdc++ -lm -lrxmaptools -lbwtools -lrxmapbase -lbwrep -lrxfile -lstorm -ljpeg
