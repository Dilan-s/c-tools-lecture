DEST    =       $(TOOLDIR)
LIBDIR  =       $(DEST)/lib/$(ARCH)
INCDIR  =       $(DEST)/include
CFLAGS  =       -I. -I$(INCDIR) -g -UDEBUGGING -Wall
LDLIBS  =       -L$(LIBDIR)
TESTEXECS	=	ctest
AUTOCRAP	=	ctest.o cx.[och] .cx.dd

all:	$(TESTEXECS)

clean:
	/bin/rm -f *.o a.out core $(TESTEXECS) $(AUTOCRAP)

cx.c cx.h:	cdata.in ../datadec
	../datadec -f cx cdata.in

ctest:		ctest.o cx.o
cx.o:		cx.h
ctest.o:	cx.h
