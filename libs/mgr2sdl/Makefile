OBJ = mgr2sdl_limo.o endisabl.o fpics.o geo.o line.o main.o pics.o plot.o event.o
CC = gcc -g `sdl-config --cflags`

#libmgr2sdl.a : $(OBJ)
#	ar cr libmgr2sdl.a $(OBJ)

mgr2sdl_limo.so: $(OBJ)
	gcc -shared $(OBJ) -o mgr2sdl_limo.so

.PHONY: clean
clean :
	rm -f *.[oa]
