CC = gcc
CFLAGS = -g -Wall -std=c11

#Address at which com program starts executing in msdos.
ORG = 256

TARGETS =       block.com key-int.com poll.com

all:		$(TARGETS)

.SUFFIXES:      .com

#Building a msdos .com using linux as.  First run as to produce standard
#linux .o.  Then use objcopy to convert to simple binary image.  But
#because of .org 0x100 directive, image has extra 256 bytes at start,
#so use dd to strip off those extra bytes
%.com:		%.s
		as --32 -a=$*.lst $< -o $*.o
		objcopy -O binary $*.o $*.bin
		rm $*.o
		dd if=$*.bin of=$@ bs=$(ORG) skip=1
		rm $*.bin

clean:
		rm -f *.com *.lst *.o *~
