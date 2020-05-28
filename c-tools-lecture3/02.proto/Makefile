DEST	=	$(TOOLDIR)
BINDIR	=	$(DEST)/bin
ARCHDIR	=	$(BINDIR)/$(ARCH)
MANDIR	=	$(DEST)/man/man1
CFLAGS	=	-g -Wall
CC	=	gcc

all:	proto

install:	proto
	install -m0755 proto $(ARCHDIR)
	install -m0644 proto.1 $(MANDIR)

clean:
	/bin/rm -f proto core *.o a.out

proto:	proto.o

proto.o:	proto.c
