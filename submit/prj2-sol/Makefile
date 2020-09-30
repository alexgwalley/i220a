CC = gcc
#CC = @touch $@; echo "$@: $?;;" gcc
CFLAGS = -g -Wall -std=c11 

BCD_BASE = 2

#testing control: can be overridden from make command-line
TEST = 0
BCD_TEST_TRACE = 0
DUMP_TEST_DATA = 0

CHECK_LIBS = -lcheck -lm -lrt -lpthread -lsubunit

OBJ_FILES = \
  bcd.o \
  main.o

PROJECT = prj2

TARGETS = 		bcd bcd-0 bcd-1 bcd-2 bcd-3 bcd-4
CHECKS = 		check-0.tst check-1.tst check-2.tst \
			  check-3.tst check-4.tst

all:			$(TARGETS)

check:			$(CHECKS)

bcd:			bcd-$(BCD_BASE)
			ln -s -f $< $@

main-%.o::		main.c bcd.h
			$(CC) $(CFLAGS) -DBCD_BASE=$* -c $< -o $@

obj-bcd-%.o:: 	        bcd.c bcd.h
			$(CC) $(CFLAGS) -DBCD_BASE=$* -c $< -o $@

test-%.o::		bcd-test.c bcd.h	
			$(CC) $(CFLAGS) \
			  -DBCD_BASE=$* \
			  -DBCD_TEST_TRACE=$(BCD_TEST_TRACE) \
			  -DTEST=$(TEST) \
			  -DDUMP_TEST_DATA=$(DUMP_TEST_DATA) \
			  -c $< -o $@

bcd-%:			main-%.o obj-bcd-%.o
			$(CC) $? -o $@

check-%.tst:		test-%.tst
			./$<

test-%.tst:		test-%.o obj-bcd-%.o	
			$(CC) $? $(CHECK_LIBS) -o $@

.PHONY:			clean
clean:
			rm -f $(TARGETS) $(CHECKS) *.o *~ *.tst
