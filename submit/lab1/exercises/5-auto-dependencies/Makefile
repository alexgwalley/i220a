CC = gcc
CPPFLAGS = -g -Wall -std=c11
LDFLAGS = -lm

#produce a list of all cc files
C_FILES = $(wildcard *.c)

#produce a list of all *.o files by substituting cc in CXX_FILES with o.
OFILES = $(C_FILES:c=o)


TARGET = quadr

#default target
all:		$(TARGET)

$(TARGET):	$(OFILES)
		$(CC) $(OFILES) $(LDFLAGS) -o $@


.PHONY:		clean
clean:
		rm -rf *~ *.o $(TARGET) $(DEPDIR)


#auto-dependences
#<http://make.mad-scientist.net/papers/advanced-auto-dependency-generation/>

#dir in which to generate dependences
DEPDIR = .deps

#have DEPDIR/*.d file for each *.c file
DEPFILES = $(C_FILES:%.c=$(DEPDIR)/%.d)

#-MT $@ sets target name in dependency file
#-MMD tells compiler to generate prereqs without including system headers
#-MP adds target for each prereq
#-MF $(DEPDIR)/$*.d sets name of output dependency file
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.d

#delete built-in implicit rule for building .o from .c
%.o: %.c

#create our own implicit rule for building %.o from %.c
#| $(DEPDIR) makes DEPDIR an order only prereq
%.o:		%.c $(DEPDIR)/%.d | $(DEPDIR)
		$(CC) $(DEPFLAGS) $(CFLAGS) -c $<

#create DEPDIR if it does not exist
$(DEPDIR):
		@mkdir -p $@

#add a target to include all DEPFILES
$(DEPFILES):
include $(wildcard $(DEPFILES))
