# destination: home dir..
# DEST	=	$(HOME)
# or can install into TOOLDIR (env var, eg ~/c-tools)
DEST	=	$(TOOLDIR)
LIBDIR	=	$(DEST)/lib/$(ARCH)
INCDIR	=	$(DEST)/include
CC	=	gcc
CFLAGS	=	-I. -I$(INCDIR) -Wall -g
LDLIBS	=	-L$(LIBDIR)
LIB	=	libutils.a
LIBOBJS	=	readline.o csvsplit.o
BUILD	=	$(LIB) testcsv

all:	$(BUILD)

clean:
	rm -f *.o $(BUILD)

install:	$(LIB)
	install -m0644 $(LIB) $(LIBDIR)
	install -m0644 *.h $(INCDIR)

$(LIB):	$(LIBOBJS)
	ar rcs $(LIB) $(LIBOBJS)

csvsplit.o:	csvsplit.h
readline.o:	readline.h
testcsv:	testcsv.o csvsplit.o
