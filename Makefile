FLAGS=-g -fno-diagnostics-show-caret
#FLAGS=-pg #this flag is for profiling

all:
	$(CC) $(CFLAGS) $(FLAGS) -c ini.c
	$(CC) $(CFLAGS) $(FLAGS) -c hash.c
	$(CC) $(CFLAGS) $(FLAGS) -c ddcfg.c
	$(CC) $(CFLGAS) $(FLAGS) -c main.c
	$(CC) $(FLAGS) ini.o hash.o ddcfg.o main.o -o main.elf
	$(CC) $(FLAGS) spec.c -o spec

clean:
	rm -f *.o *.elf *~
