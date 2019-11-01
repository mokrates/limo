OBJ = simplesdl.o endisabl.o fpics.o geo.o line.o main.o pics.o plot.o event.o
CC = gcc -g -fPIC `sdl-config --cflags` -I../..

simplesdl.a : $(OBJ) Makefile
	$(CC) -DLIMO_MOD_INLINE -c simplesdl.c
	ar cr simplesdl.a $(OBJ)

simplesdl.so: $(OBJ) Makefile
	$(CC) -c simplesdl.c
	gcc -o simplesdl.so -shared $(OBJ) `sdl-config --libs`

.PHONY: clean
clean :
	rm -f *.[oa]
	rm -f *~
	rm -f *.so

realclean: clean
