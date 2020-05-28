# destination: home dir..
# DEST		=	$(HOME)
# or can install into TOOLDIR (env var, eg ~/c-tools)
DEST		=	$(TOOLDIR)
# or can install systemwide, with optional $(PREFIX) support for packaging
# DEST		=	$(PREFIX)/usr
# or can install via debian package:
# DEST		=	$(DESTDIR)/usr
BINDIR		=	$(DEST)/bin
ARCHDIR		=	$(BINDIR)/$(ARCH)
# do you want "share" or not - yes for packaging, no for most other installs
#MANDIR		=	$(DEST)/share/man/man1
MANDIR		=	$(DEST)/man/man1
CC		=	gcc
CFLAGS		=	-g -UDEBUGGING -Wall
LDLIBS		=	
EXECS		=	datadec

datadec_srcs	=	datadec.c parser.c lexer.c struct.c decs.c optimize.c set.c
datadec_objs	=	datadec.o parser.o lexer.o struct.o decs.o optimize.o set.o


all:	$(EXECS)

clean:
	/bin/rm -f *.o a.out core $(EXECS)
	cd test; make clean

install:	$(EXECS)
	#install -m 755 $(EXECS) $(BINDIR)
	install -m 755 $(EXECS) $(ARCHDIR)
	mkdir -p $(MANDIR)
	#chmod 755 $(MANDIR)
	install -m 644 c-datadec.man $(MANDIR)/datadec.1

datadec:	$(datadec_objs)

datadec.o:      struct.h lexer.h parser.h decs.h optimize.h set.h
decs.o:         struct.h decs.h
lexer.o:        struct.h lexer.h
optimize.o:	optimize.h struct.h
parser.o:       struct.h lexer.h parser.h
struct.o:       struct.h
set.o:       	set.h
