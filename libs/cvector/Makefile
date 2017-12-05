LIBNAME=cvector
OBJ = $(LIBNAME).o 
CFLAGS += -I../..
CC=gcc $(CFLAGS)

$(LIBNAME).so: $(OBJ) Makefile
	$(CC) -o $(LIBNAME).so -shared $(OBJ)

cvector.o: cvector.c
	$(CC) -fPIC -c cvector.c

.PHONY: clean
clean :
	rm -f *.[oa] *~

realclean: clean
	rm -f $(LIBNAME).so
