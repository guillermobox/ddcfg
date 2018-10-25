CFLAGS=-g -Wall

all: ddcfg.a

.PHONY: test clean

test: ddcfg.a
	cd test && make

ddcfg.a: ast.o ddcfg.o expparser.tab.o lex.yy.o hash.o ini.o spec.o utilities.o
	ar -rc $@ $^

expparser.tab.c: expparser.y expparser.l
	lex -s expparser.l
	bison -d expparser.y

%.o: %.c
	$(CC) -c -g $^ -o $@

clean:
	rm -f *.o *.a wizard
