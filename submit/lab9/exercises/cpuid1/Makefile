TARGET = cpuid

CFLAGS = -g -Wall -std=c11 -Og

$(TARGET):	main.o cpuid.o
		$(CC) main.o cpuid.o -o $@

cpuid.o:	cpuid.s
		$(CC) -c $(CFLAGS) $< -o $@

.PHONY:		clean
clean:
		rm -f *~ *.o $(TARGET)
