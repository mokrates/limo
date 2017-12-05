OBJ = mgr2sdl_limo.o endisabl.o fpics.o geo.o line.o main.o pics.o plot.o event.o
CC = gcc -g -fPIC `sdl-config --cflags` -I../..

#libmgr2sdl.a : $(OBJ)
#	ar cr libmgr2sdl.a $(OBJ)

mgr2sdl_limo.so: $(OBJ) Makefile
	gcc -o mgr2sdl_limo.so -shared $(OBJ) `sdl-config --libs`

.PHONY: clean
clean :
	rm -f *.[oa]

realclean: clean
