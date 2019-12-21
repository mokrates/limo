LIBNAME=threading
OBJ = $(LIBNAME).o 
CFLAGS += -I../.. -fPIC
CC=gcc $(CFLAGS)

$(LIBNAME).so: $(OBJ) Makefile
	$(CC) -o $(LIBNAME).so -shared $(OBJ) -lpthread

$(LIBNAME).a: $(OBJ) Makefile
	ar rcs $(LIBNAME).a $(OBJ)

.PHONY: clean
clean :
	rm -f *.[oa] *~

realclean: clean
	rm -f $(LIBNAME).so
