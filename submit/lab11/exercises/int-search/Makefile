CFLAGS = -g -Wall -std=c11 -O1

TARGETS = 	linear-search binary-search

all:		$(TARGETS)

binary-search:	main.o int_compare.o binary-search.o
linear-search:	main.o int_compare.o linear-search.o

.PHONY:		clean
clean:	
		rm -f $(TARGETS) *.o *~
