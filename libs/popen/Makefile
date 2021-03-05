LIBNAME=popen
OBJ = $(LIBNAME).o 
CFLAGS += -I../..
CC=gcc $(CFLAGS)

$(LIBNAME).so: $(OBJ) Makefile
	$(CC) -o $(LIBNAME).so -shared $(OBJ)

$(LIBNAME).a: $(OBJ) Makefile
	ar rcs popen.a popen.o

popen.o: popen.c
	$(CC) -fPIC -c popen.c

.PHONY: clean
clean :
	rm -f *.[oa] *~

realclean: clean
	rm -f $(LIBNAME).so
