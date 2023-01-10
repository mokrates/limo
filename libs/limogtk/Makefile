LIBNAME=limogtk
OBJ = $(LIBNAME).o
CFLAGS += -Wno-deprecated-declarations -Wno-discarded-qualifiers -g -I../.. `pkg-config gtk+-3.0 --cflags` -fPIC
#CFLAGS += -Wno-deprecated-declarations -Wno-discarded-qualifiers -g -I../.. `i686-w64-mingw32-pkg-config gtk+-win32-3.0 --cflags` -fPIC

$(LIBNAME).so: $(OBJ) Makefile
	$(CC) $(CFLAGS) -o $(LIBNAME).so -shared $(OBJ) `pkg-config gtk+-3.0 --libs`

$(LIBNAME).a: $(OBJ) Makefile
	$(AR) rcs $(LIBNAME).a $(OBJ)

.PHONY: clean
clean :
	rm -f *.[oa] *~ _*

realclean: clean
	rm -f $(LIBNAME).so

$(LIBNAME).o: _gtk-inserts.h $(LIBNAME).c gtk-marshall.h
	$(CC) $(CFLAGS) -c $(LIBNAME).c

_gtk-inserts.h:
	bash generate_stuff.sh
