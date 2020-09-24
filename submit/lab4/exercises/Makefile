CC = gcc
CPPFLAGS = -g -Wall -Wextra -Wno-unused-parameter -std=c11
LDFLAGS = -lm

#produce a list of all cc files
C_FILES = $(wildcard *.c)

#produce a list of all *.o files by substituting cc in CXX_FILES with o.
OUTFILES = $(C_FILES:.c=)


TARGET = $(OUTFILES)

#default target
all:		$(TARGET)


.PHONY:		clean
clean:
		rm -rf *~ *.o $(TARGET)

.PHONY:		.gitignore
.gitignore:
		rm -f $@
		for f in $(OUTFILES) ; \
		do \
		  echo $$f >> $@ ; \
		done
