CFLAGS = -g -Wall -std=c11 -O1

TARGETS = parity-c parity-s

all:		$(TARGETS)

parity-c:	main.o parity-c.o
parity-s:	main.o parity-s.o

.PHONY:		clean
clean:	
		rm -f $(TARGETS) *.o *~
