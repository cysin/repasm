########################################################################
#
# Makefile for compiling StormLib under linux
#
# Author: Marko Friedemann <marko.friedemann@bmx-chemnitz.de>
# Created at: Mon Jan 29 18:26:01 CEST 2001
# Computer: whiplash.flachland-chemnitz.de 
# System: Linux 2.4.0 on i686
#    
# Copyright (c) 2001 BMX-Chemnitz.DE All rights reserved.
#
########################################################################

FILES.cpp = SCommon.cpp SCompression.cpp SFileCompactArchive.cpp \
	SFileCreateArchiveEx.cpp SFileExtractFile.cpp SFileFindFile.cpp \
	SListFile.cpp SFileOpenArchive.cpp SFileOpenFileEx.cpp \
        SAttrFile.cpp SFileReadFile.cpp \
	StormPortLinux.cpp wave/wave.cpp huffman/huff.cpp \
	pklib/crc32.cpp pklib/explode.cpp pklib/implode.cpp \
	misc/crc32.cpp misc/md5.cpp
FILESbz.c = bzip2/blocksort.c bzip2/huffman.c bzip2/crctable.c bzip2/randtable.c bzip2/compress.c bzip2/decompress.c bzip2/bzlib.c
FILESz.c =  zlib/adler32.c zlib/compress.c zlib/crc32.c zlib/gzio.c zlib/uncompr.c zlib/deflate.c \
			zlib/trees.c  zlib/zutil.c zlib/inflate.c \
			zlib/infback.c zlib/inftrees.c zlib/inffast.c 
FILESbz.o = $(FILESbz.c:.c=.o)
FILESz.o = $(FILESz.c:.c=.o)

FILES.o = $(FILES.cpp:.cpp=.o)

include ../config

LIB = libStorm.so

CXX = g++
CFLAGS = -s -D__SYS_ZLIB
I_FLAGS = 
LDFLAGS = -lz -lbz2

all: $(LIB)

$(LIB): $(FILES.o) $(FILESbz.o) $(FILESz.o)
	ar cr ../libs/libstorm.a $(FILES.o) $(FILESbz.o) $(FILESz.o)

%.o: %.cpp
	$(CXX) -D BZ_STRICT_ANSI -I../include $(CFLAGS) -c $< -o $@

%.o: %.c
	$(CC) -D BZ_STRICT_ANSI $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(FILES.o) $(FILESbz.o) $(FILESz.o) $(LIB)

new: clean all

mrproper: clean
	$(RM) Makefile.deps

mrnew: mrproper new

install: $(LIB)
	install $(I_FLAGS) $(LIB) /usr/local/lib
	mkdir -p /usr/local/include/StormLib
	cp Storm.h /usr/local/include/StormLib
	cp StormPort.h /usr/local/include/StormLib
	ldconfig

deps:
	$(CXX) -MM $(CFLAGS) $(FILES.cpp) > Makefile.deps
	
-include Makefile.deps

.PHONY: all clean new mrpoper mrnew install deps
