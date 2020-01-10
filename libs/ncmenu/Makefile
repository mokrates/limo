LIBNAME=ncmenu
OBJ = $(LIBNAME).o
CFLAGS += -Wall -g -I../.. -fPIC 
CC=gcc

$(LIBNAME).so: $(OBJ) Makefile
	$(CC) -o $(LIBNAME).so -shared $(OBJ) -lmenu -lcurses

$(LIBNAME).a: $(OBJ) Makefile
	ar rcs $(LIBNAME).a $(OBJ)

.PHONY: clean
clean :
	rm -f *.[oa] *~

realclean: clean
	rm -f $(LIBNAME).so
