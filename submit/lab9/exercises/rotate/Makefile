CFLAGS = -Wall -Og -std=c11

BASE = rotate

TARGETS = \
  rotate.s

all:		$(TARGETS)

%.s:		%.c
		$(CC) -S $(CFLAGS) $<


.PHONY:		clean
clean:
		rm -f *~ *.o $(TARGETS)


