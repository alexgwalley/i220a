CFLAGS = -g -Wall -std=c11

multiple-symbols:	def1.o def2.o main.o
			$(CC) def1.o def2.o main.o $(LDLIBS) -o $@

.PHONY:		clean
clean:
		rm -f *.o multiple-symbols

