TARGET=stall-sim
CC=gcc
COURSE=cs220
IFLAGS= -I $$HOME/$(COURSE)/include
LDFLAGS = -L $$HOME/$(COURSE)/lib -l cs220 -l y86

$(TARGET): main.o stall-sim.o
	$(CC) $(LDFLAGS) main.o stall-sim.o -o $(TARGET)

%.o: %.c
	$(CC) -c $< $(IFLAGS)

clean:
	rm -f *.o
	rm stall-sim
