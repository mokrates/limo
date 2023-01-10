LIBNAME=ncurses
OBJ = $(LIBNAME).o
CFLAGS += `pkg-config ncurses --cflags` -Wall -g -I../.. -fPIC 

$(LIBNAME).so: $(OBJ) Makefile
	$(CC) $(CFLAGS) -o $(LIBNAME).so -shared $(OBJ) `pkg-config ncurses --libs`

$(LIBNAME).dll: $(OBJ) Makefile
	$(CC) $(CFLAGS) -o $(LIBNAME).dll -shared  ../../limodll.dll $(OBJ) -lgc -lgmp `pkg-config ncurses --libs`

$(LIBNAME).a: $(OBJ) Makefile
	$(AR) rcs $(LIBNAME).a $(OBJ) `pkg-config ncurses --static`

.PHONY: clean
clean :
	rm -f *.[oa] *~

realclean: clean
	rm -f $(LIBNAME).so
