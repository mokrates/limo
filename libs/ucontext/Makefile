LIBNAME=ucontext
OBJ = $(LIBNAME).o
CFLAGS +=-g -I../.. -fPIC
CC=gcc

$(LIBNAME).so: $(OBJ) Makefile
	$(CC) -o $(LIBNAME).so -shared $(OBJ)

$(LIBNAME).a: $(OBJ) Makefile
	ar rcs $(LIBNAME).a $(OBJ)

.PHONY: clean
clean :
	rm -f *.[oa] *~ *.so

realclean: clean
	rm -f $(LIBNAME).so
