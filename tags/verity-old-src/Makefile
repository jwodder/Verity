OBJS = verity.tab.o lex.yy.o veritypes.o veriprint.o
CFLAGS = -std=c99 -O2

verity : $(OBJS)
	cc -o verity $(OBJS)
verity.tab.o lex.yy.o veritypes.o veriprint.o : veritypes.h
lex.yy.o veritypes.o veriprint.o : verity.tab.h
verity.tab.o veriprint.o : veriprint.h
verity.tab.c : verity.y
	bison -d verity.y
verity.tab.h : verity.y
	bison -d verity.y
lex.yy.c : verity.l
	flex verity.l

.PHONY : clear
clear :
	-rm -f *.o verity.tab.[ch] lex.yy.c
