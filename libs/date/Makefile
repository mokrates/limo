LIBNAME=date
OBJ = $(LIBNAME).o 
CFLAGS += -I../.. -fPIC -g
CC=gcc $(CFLAGS)

$(LIBNAME).so: $(OBJ) Makefile
	$(CC) -o $(LIBNAME).so -shared $(OBJ)

$(LIBNAME).a: $(OBJ) Makefile
	ar rcs $(LIBNAME).a $(OBJ)

.PHONY: clean
clean :
	rm -f *.[oa] *~

realclean: clean
	rm -f $(LIBNAME).so
