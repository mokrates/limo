obj = load.o set.o write.o

CC = gcc `sdl-config --cflags`

all: libfonts.a
	mv libfonts.a ..

libfonts.a : $(obj)
	ar cr libfonts.a $(obj)

clean:
	rm -f *.[oa]