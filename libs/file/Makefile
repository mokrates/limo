LIBNAME=file
OBJ = $(LIBNAME).o 
CFLAGS += -I../..
CC=gcc $(CFLAGS)

$(LIBNAME).so: $(OBJ) Makefile
	$(CC) -o $(LIBNAME).so -shared $(OBJ)

$(LIBNAME).a: $(OBJ) Makefile
	ar rcs file.a file.o

file.o: file.c
	$(CC) -fPIC -c file.c

.PHONY: clean
clean :
	rm -f *.[oa] *~

realclean: clean
	rm -f $(LIBNAME).so
