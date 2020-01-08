LIBNAME=sdl
OBJ = $(LIBNAME).o surface.o surfacecolor.o plot.o line.o text.o
CFLAGS += -Wall -g -I../.. -fPIC `sdl-config --cflags`
CC=gcc

$(LIBNAME).so: $(OBJ) Makefile
	$(CC) -o $(LIBNAME).so -shared $(OBJ) `sdl-config --libs`

$(LIBNAME).a: $(OBJ) Makefile
	ar rcs $(LIBNAME).a $(OBJ)

.PHONY: clean
clean :
	rm -f *.[oa] *~

realclean: clean
	rm -f $(LIBNAME).so
