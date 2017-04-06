include ../config

all: librxmaptools.a


FILES.cpp =	RXLQMapconverter.cpp \
			RXHQMapConverter.cpp \
			RXLQUnitLayer.cpp \
			RXRenderableUnitContainer.cpp\
			RXBitmapJPEG.cpp\
			RXUnitRenderer.cpp	\
			RXUnitLayerImpl.cpp\
			RXUnitLayer.cpp\
			RXUnitDAT.cpp\
			RXScriptOpcodeReader.cpp\
			RXRenderableUnit.cpp\
			RXRenderableMap.cpp\
			RXMapConverter.cpp\
			RXLOLoader.cpp\
			RXLO.cpp\
			RXIScript.cpp\
			RXImageDAT.cpp\
			RXBitmapCache.cpp\
			RXMinimap.cpp\
			RXMapGenerator.cpp\
			RXUnitLoader.cpp\
			RXTilesetLoader.cpp\
			RXTileset.cpp\
			RXPalette.cpp\
			RXGRPLoader.cpp\
			RXWPE.cpp\
			RXVX4.cpp\
			RXVR4.cpp\
			RXTBL.cpp\
			RXGRP.cpp\
			RXDAT.cpp\
			RXCV5.cpp\
			RXTeamPalette.cpp\
			RXShadowPalette.cpp\
			RXBitmap.cpp

FILES.o = $(FILES.cpp:.cpp=.o)

%.o: %.cpp
	$(CPP) $(CCFLAGS) -I$(HEADERS) -I$(LIBJPEGDIR) -c $< -o $@
	
	
librxmaptools.a:	$(FILES.o)
	$(AR) cr ../libs/librxmaptools.a $(FILES.o)
 

clean:
	$(RM) *.o *.a