LIBNAME=ncmenu
OBJ = $(LIBNAME).o
CFLAGS += -Wall -g -I../.. -fPIC `pkg-config --cflags menu`
CC=gcc

$(LIBNAME).so: $(OBJ) Makefile
	$(CC) $(CFLAGS) -o $(LIBNAME).so -shared $(OBJ) `pkg-config --libs menu` `pkg-config --libs ncurses`

$(LIBNAME).dll: $(OBJ) Makefile
	$(CC) $(CFLAGS) -o $(LIBNAME).dll -shared  ../../limodll.dll $(OBJ) -lgc -lgmp `pkg-config --libs menu` `pkg-config --libs ncurses`

$(LIBNAME).a: $(OBJ) Makefile
	$(AR) rcs $(LIBNAME).a $(OBJ) `pkg-config menu --static` `pkg-config ncurses --static`

.PHONY: clean
clean :
	rm -f *.[oa] *~

realclean: clean
	rm -f $(LIBNAME).so
