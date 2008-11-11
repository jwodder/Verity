# $Id$

OBJS = verity.tab.o verilex.o veritypes.o veriprint.o
CFLAGS = -std=c99 -O2

.PHONY : all clear
all : verity verity.1

verity : $(OBJS)
	cc -o verity $(OBJS)
verity.tab.o verilex.o veritypes.o veriprint.o : veritypes.h
verilex.o veritypes.o veriprint.o : verity.tab.h
verity.tab.o veriprint.o : veriprint.h
verity.tab.c verity.tab.h : verity.y
	bison -d verity.y

verity.1 : verity.pod
	pod2man -c '' -r 'Version 1.2.1' verity.pod verity.1
clear :
	-rm -f *.o verity.tab.[ch]
