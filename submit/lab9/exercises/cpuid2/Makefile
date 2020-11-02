TARGET = cpuid

#CFLAGS = -g -Wall -std=c11 -Og
#-Og results in funky behavior (gdb reports op "optimized out")
CFLAGS = -g -Wall -std=c11 

$(TARGET):	main.o cpuid.o
		$(CC) main.o cpuid.o -o $@

cpuid.o:	cpuid.s
		$(CC) -c $(CFLAGS) $< -o $@

.PHONY:		clean
clean:
		rm -f *~ *.o $(TARGET)
