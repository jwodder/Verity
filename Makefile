OBJS = verity.tab.o verilex.o veritypes.o veriprint.o
CC = c99
CFLAGS = -O2

.PHONY : all clean
all : verity verity.1

verity : $(OBJS)
	$(CC) -o verity $(OBJS)
verity.tab.o verilex.o veritypes.o veriprint.o : veritypes.h
verilex.o veritypes.o veriprint.o : verity.tab.h
verity.tab.o veriprint.o : veriprint.h

verity.tab.c verity.tab.h : verity.y
	bison -d verity.y

# Note: Bison (not Yacc) is required for compiling as it enables location
# tracking for error messages and, more importantly, produces syntax error
# messages that are actually helpful.  If you want/need to use Yacc instead,
# you'll have to comment out a fair bit of code (mostly anything that mentions
# yylloc).

verity.1 : verity.pod
	pod2man -c '' -r 'Version 1.4' verity.pod verity.1

clean :
	-rm -f *.o verity.tab.[ch]
