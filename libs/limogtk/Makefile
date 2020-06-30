LIBNAME=limogtk
OBJ = $(LIBNAME).o
CFLAGS += -Wno-discarded-qualifiers -g -I../.. `pkg-config gtk+-3.0 --cflags` -fPIC
CC=gcc

$(LIBNAME).so: $(OBJ) Makefile
	$(CC) -o $(LIBNAME).so -shared $(OBJ) `pkg-config gtk+-3.0 --libs`

$(LIBNAME).a: $(OBJ) Makefile
	ar rcs $(LIBNAME).a $(OBJ)

.PHONY: clean
clean :
	rm -f *.[oa] *~ _*

realclean: clean
	rm -f $(LIBNAME).so

$(LIBNAME).o: _gtk-inserts.h $(LIBNAME).c gtk-marshall.h
	$(CC) $(CFLAGS) -c $(LIBNAME).c

_gtk-inserts.h:
	bash generate_stuff.sh
