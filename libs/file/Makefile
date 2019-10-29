LIBNAME=file
OBJ = $(LIBNAME).o 
CFLAGS += -I../..
CC=gcc $(CFLAGS)

$(LIBNAME).so: $(OBJ) Makefile
	$(CC) -o $(LIBNAME).so -shared $(OBJ)

$(LIBNAME).a: file.c Makefile
	$(CC) -DLIMO_MOD_INLINE -c file.c
	ar rcs file.a file.o

file.o: file.c
	$(CC) -fPIC -c file.c

.PHONY: clean
clean :
	rm -f *.[oa] *~

realclean: clean
	rm -f $(LIBNAME).so
