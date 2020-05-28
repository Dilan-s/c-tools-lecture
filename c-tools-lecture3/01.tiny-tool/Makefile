DEST	=	$(TOOLDIR)
BINDIR	=	$(DEST)/bin
ARCHDIR	=	$(BINDIR)/$(ARCH)
MANDIR	=	$(DEST)/man/man1
CC	=	gcc
CFLAGS	=	-Wall -g
BUILD	=	genfuncs1 genfuncs2 genfuncs3

all:	$(BUILD)

install:	tmpl
	install -m0755 tmpl $(BINDIR)
	install -m0755 tmpl.man $(MANDIR)/tmpl.1

clean:
	/bin/rm -f $(BUILD) *.o core

genfuncs1:	genfuncs1.o
genfuncs2:	genfuncs2.o
genfuncs3:	genfuncs3.o
