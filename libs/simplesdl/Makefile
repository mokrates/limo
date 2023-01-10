LIBNAME=simplesdl
OBJ = simplesdl.o endisabl.o fpics.o geo.o line.o main.o pics.o plot.o event.o
CC = gcc -g -fPIC `sdl-config --cflags` -I../..

$(LIBNAME).a : $(OBJ) Makefile
	ar cr $(LIBNAME).a $(OBJ)

$(LIBNAME).so: $(OBJ) Makefile
	$(CC) -c $(LIBNAME).c
	gcc -o $(LIBNAME).so -shared $(OBJ) `sdl-config --libs`

$(LIBNAME).dll: $(OBJ) Makefile
	$(CC) $(CFLAGS) -o $(LIBNAME).dll -shared  ../../limodll.dll $(OBJ) -lgc -lgmp `sdl-config --libs`

.PHONY: clean
clean :
	rm -f *.[oa]
	rm -f *~
	rm -f *.so

realclean: clean
