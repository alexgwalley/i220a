CFLAGS = -g -Wall  -std=c11
LDFLAGS = -lm

all:		log10-static log10-dynamic

log10-dynamic:	log10.o
		$(CC) $< $(LDFLAGS) -o $@

log10-static:	log10.o
		$(CC) -static $< $(LDFLAGS) -o $@

.PHONY:		clean
clean:
		rm -f *.o log10-static log10-dynamic

