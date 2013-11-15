
all:
	$(CC) $(CFLAGS) -c ini.c
	$(CC) $(CFLAGS) -c hash.c
	$(CC) $(CFLAGS) -c cfg.c
	$(CC) $(CFLGAS) ini.o cfg.o hash.o main.c -o main.elf

clean:
	rm -f *.o *.elf
