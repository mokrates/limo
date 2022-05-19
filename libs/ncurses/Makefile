LIBNAME=ncurses
OBJ = $(LIBNAME).o
CFLAGS += `pkg-config ncurses --cflags` -Wall -g -I../.. -fPIC 
CC=gcc

$(LIBNAME).so: $(OBJ) Makefile
	$(CC) -o $(LIBNAME).so -shared $(OBJ) `pkg-config ncurses --libs`

$(LIBNAME).a: $(OBJ) Makefile
	ar rcs $(LIBNAME).a $(OBJ) `pkg-config ncurses --static`

.PHONY: clean
clean :
	rm -f *.[oa] *~

realclean: clean
	rm -f $(LIBNAME).so
