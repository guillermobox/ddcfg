CFLAGS=-g -fno-diagnostics-show-caret

all: ddcfg.a

ddcfg.a: ini.o hash.o ddcfg.o spec.o
	ar -rc $@ $^

clean:
	rm -f *.o *.a
