LIBNAME=vigra_limo
OBJ = $(LIBNAME).o libvigra_c.so
CFLAGS += -I../.. -I../../../vigra_c/vigra_c/src
CC=g++ -Wno-write-strings $(CFLAGS)

$(LIBNAME).so: $(OBJ) Makefile
	$(CC) -L. -o $(LIBNAME).so -shared $(OBJ) -Wl,-rpath,/usr/local/lib/limo -l :libvigra_c.so

vigra_limo.o: vigra_limo.c
	$(CC) -fPIC -c vigra_limo.c

libvigra_c.so:
	cp ../../../vigra_c/vigra_c/bin/libvigra_c.so .

.PHONY: clean
clean :
	rm -f *.[oa] *~

realclean: clean
	rm -f $(LIBNAME).so
