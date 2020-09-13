TARGET = hamming-encode
OBJS =\
	main.o \
	hamming.o
CC = gcc
CFLAGS = -std=c11 -g -Wall
LDFLAGS = 

$(TARGET):	$(OBJS)
		$(CC) $(OBJS) $(LDFLAGS) -o $@

main.o:		main.c hamming.h
		$(CC) $(CFLAGS) -c $<
hamming.o:	hamming.c hamming.h
		$(CC) $(CFLAGS) -c $<


hamming-decode:	hamming-encode
		ln -s -f $< $@

clean:		
		rm -f *~ *.o $(TARGET)
