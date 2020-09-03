PROJECT = key-value
SRC_FILES = key-value.c \
	    key-value.h \
	    process-file.c \
	    process-file.h \
	    main.c
CFLAGS = -g -Wall -std=c11
LDFLAGS = 

$(PROJECT):	key-value.o process-file.o main.o 
		$(CC) key-value.o process-file.o main.o $(LDFLAGS) -o $@
depend:
		$(CC) -MM $(CPPFLAGS) *.c
clean:
		rm -f *.o $(PORJECT) $(PROJECT).tar.gz
dist:		$(PROJECT).tar.gz
$(PROJECT).tar.gz:	$(SRC_FILES)
			tar -cf $(PROJECT) $(PROJECT).tar.gz
			gzip -f $(PROJECT).tar
		
#automatically generate dependencies produced by make depend
key-value.o: key-value.c key-value.h
main.o: main.c process-file.h
process-file.o: process-file.c key-value.h process-file.h
