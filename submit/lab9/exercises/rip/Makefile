CFLAGS = -Wall -Og -std=c11

BASE = rip

TARGETS = \
  $(BASE).s \
  $(BASE) \
  $(BASE).dump

all:		$(TARGETS)

# $(BASE): 	$(BASE).c

%.s:		%.c
		$(CC) $(CFLAGS) -S $<

%.dump:		%
		objdump -d $< > $@


.PHONY:		clean
clean:
		rm -f *~ *.o $(TARGETS)


