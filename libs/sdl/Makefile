LIBNAME=sdl
OBJ = $(LIBNAME).o surface.o surfacecolor.o plot.o line.o text.o clearsurface.o blit.o events.o
CFLAGS += -Wall -g -I../.. -fPIC `sdl-config --cflags`
CC=gcc

$(LIBNAME).so: $(OBJ) Makefile
	$(CC) $(CFLAGS) -o $(LIBNAME).so -shared $(OBJ) `sdl-config --libs` -lSDL_ttf

$(LIBNAME).dll: $(OBJ) Makefile
	$(CC) $(CFLAGS) -o $(LIBNAME).dll -shared  ../../limodll.dll $(OBJ) -lgc -lgmp `sdl-config --libs` -lSDL_ttf

$(LIBNAME).a: $(OBJ) Makefile
	$(AR) rcs $(LIBNAME).a $(OBJ)

.PHONY: clean
clean :
	rm -f *.[oa] *~

realclean: clean
	rm -f $(LIBNAME).so
