CFLAGS=-g -Wall

all: ddcfg.a

.PHONY: test clean

test: ddcfg.a
	cd test && make

ddcfg.a: ddcfg-merged.o
	ar -rc $@ $^

ddcfg-merged.o: $(wildcard *.c)

clean:
	rm -f *.o *.a wizard
