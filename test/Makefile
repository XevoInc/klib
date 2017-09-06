CC=gcc
CXX=g++
CFLAGS=-g -Wall -O2 -I..
CXXFLAGS=$(CFLAGS)
PROGS=khash_keith khash_keith2 khash_test kvec_test

all:$(PROGS)

clean:
		rm -fr $(PROGS) *.dSYM a.out *.o

khash_keith:khash_keith.c
		$(CC) $(CFLAGS) -o $@ khash_keith.c

khash_keith2:khash_keith2.c
		$(CC) $(CFLAGS) -o $@ khash_keith2.c

khash_test:khash_test.c
		$(CC) $(CFLAGS) -o $@ khash_test.c

kvec_test:kvec_test.cc
		$(CXX) $(CXXFLAGS) -o $@ kvec_test.cc
