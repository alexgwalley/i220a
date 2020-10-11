CFLAGS = -g -Wall -fPIC -std=c11

OBJS = \
  addvec.o \
  multvec.o

all:		libvec.so  libvec.a testvec-static testvec-dynamic

libvec.so:	$(OBJS)
		$(CC) -shared $(OBJS) -o $@

libvec.a:	$(OBJS)
		ar rcs $@ $(OBJS)

testvec-static: testvec.o
		$(CC)  -static testvec.o -L. -lvec -o $@

testvec-dynamic: testvec.o
		$(CC) testvec.o -L. -lvec -o $@

.PHONY:		clean
clean:
		rm -f *.o *.so *.a testvec-*
