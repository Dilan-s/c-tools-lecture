# destination: home dir..
# DEST	=	$(HOME)
# or can install into TOOLDIR (env var, eg ~/c-tools)
DEST	=	$(TOOLDIR)
LIBDIR	=	$(DEST)/lib/$(ARCH)
INCDIR	=	$(DEST)/include
CC	=	gcc
CFLAGS  =       -Wall -I$(INCDIR)
LDLIBS  =       -L$(LIBDIR) -ltestlib
LIB	=	libADTs.a
LIBOBJS	=	hash.o longhash.o set.o longset.o strlist.o wordlist.o intlist.o
TBUILD	=	testhash testlonghash testset testlongset teststrlist testwordlist testintlist
BUILD	=	$(TBUILD) iterate longiterate wordset $(LIB)

all:	$(BUILD)

clean:
	/bin/rm -f $(BUILD) *.o core

test:	$(TBUILD)
	summarisetests --max 20 $(TBUILD)

install:	$(LIB)
	install -m0644 $(LIB) $(LIBDIR)
	install -m0644 *.h $(INCDIR)

$(LIB):	$(LIBOBJS)
	rm -f $(LIB)
	ar rcs $(LIB) $(LIBOBJS)

testwordlist:	testwordlist.o wordlist.o
teststrlist:	teststrlist.o strlist.o
iterate:	iterate.o hash.o
longiterate:	longiterate.o longhash.o
testset:	testset.o set.o
testlongset:	testlongset.o longset.o
wordset:	wordset.o set.o
testhash:	testhash.o hash.o
testlonghash:	testlonghash.o longhash.o
testintlist:	testintlist.o intlist.o

wordlist.o:	wordlist.h
testwordlist.o:	wordlist.h
strlist.o:	strlist.h
teststrlist.o:	strlist.h
set.o:		set.h
longset.o:	longset.h
testset.o:	set.h
wordset.o:	set.h
hash.o:		hash.h
testhash.o:	hash.h
iterate.o:	hash.h
longhash.o:	longhash.h
testlonghash.o:	longhash.h
longiterate.o:	longhash.h
intlist.o:	intlist.h
testintlist.o:	intlist.h
