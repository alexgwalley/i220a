CFLAGS = -g -Wall -std=c11 -O1
LDFLAGS = 

TARGETS =		simple-matmul transpose-matmul

all:			$(TARGETS)

simple-matmul:		main.o simple-matmul.o
			$(CC) $^ $(LDFLAGS) -o $@

transpose-matmul: 	main.o transpose-matmul.o
			$(CC) $^ $(LDFLAGS) -o $@


#Removes all objects and executables.
.PHONY:			clean
clean:	
			rm -f $(TARGETS) *.o *~
