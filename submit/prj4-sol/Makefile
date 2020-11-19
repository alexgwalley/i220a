TARGET=y86-sim
CC=gcc
COURSE=cs220
IFLAGS= -I $$HOME/$(COURSE)/include
LDFLAGS = -L $$HOME/$(COURSE)/lib -l cs220 -l y86

$(TARGET): main.o ysim.o
	$(CC) $(LDFLAGS) main.o ysim.o -o $(TARGET)

%.o: %.c
	$(CC) -c $< $(IFLAGS)

clean:
	rm -f *.o
	rm y86-sim
