<<<<<<< HEAD
CFLAGS=-g -Wall
=======
CFLAGS=-g -fno-diagnostics-show-caret -Wall `pkg-config --cflags gtk+-3.0`
>>>>>>> Add support to check integers or doubles at interface

all: wizard

.PHONY: test clean

wizard.o: wizard.c
	$(CC) $(CFLAGS) `pkg-config --cflags gtk+-3.0` wizard.c -c -o wizard.o

wizard: wizard.o spec.o ddcfg.o ini.o hash.o
	$(CC) $^ `pkg-config --cflags --libs gtk+-3.0` -o $@

test: ddcfg.a
	cd test && make

ddcfg.a: ddcfg-merged.o
	ar -rc $@ $^

ddcfg-merged.o: $(wildcard *.c)

clean:
	rm -f *.o *.a wizard
