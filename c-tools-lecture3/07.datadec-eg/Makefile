# TOOLDIR should be in the environment, set to ~/c-tools
INSTDIR =       $(TOOLDIR)
INCDIR  =       $(INSTDIR)/include
LIBDIR  =       $(INSTDIR)/lib/$(ARCH)
CC      =       gcc
CFLAGS  =       -Wall -g -I$(INCDIR)
LDLIBS  =       -L$(LIBDIR) -ltestlib
BUILD	=	testintlist testtree mintesttree

all:	$(BUILD)

clean:
	/bin/rm -f $(BUILD) *.o core datatypes.c datatypes.h

datatypes.c datatypes.h:	types.in
	#datadec -f datatypes types.in
	datadec datatypes types.in

test:	$(BUILD)
	summarisetests --max 20 $(BUILD)

testintlist:	testintlist.o datatypes.o
testtree:	testtree.o datatypes.o nleaves.o
mintesttree:	mintesttree.o datatypes.o nleaves.o
datatypes.o:	datatypes.h
testintlist.o:	datatypes.h
testtree.o:	datatypes.h nleaves.h
mintesttree.o:	datatypes.h nleaves.h
nleaves.o:	datatypes.h nleaves.h
