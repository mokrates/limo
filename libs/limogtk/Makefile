LIBNAME=limogtk
OBJ = $(LIBNAME).o
CFLAGS += -Wall -g -I../.. `pkg-config gtk+-3.0 --cflags` -fPIC
CC=gcc

$(LIBNAME).so: $(OBJ) Makefile
	$(CC) -o $(LIBNAME).so -shared $(OBJ) `pkg-config gtk+-3.0 --libs`

$(LIBNAME).a: $(OBJ) Makefile
	ar rcs $(LIBNAME).a $(OBJ)

.PHONY: clean
clean :
	rm -f *.[oa] *~

realclean: clean
	rm -f $(LIBNAME).so
