LIBNAME=vigra_limo
OBJ = vigra_limo.o tensors.o filters.o morphology.o
CFLAGS += -I../.. -I../../../vigra_c/vigra_c/src
CC=g++ -Wno-write-strings $(CFLAGS)

$(LIBNAME).so: $(OBJ) libvigra_c.so
	$(CC) -L. -o $(LIBNAME).so -shared $(OBJ) -Wl,-rpath,/usr/local/lib/limo -l :libvigra_c.so

$(OBJ): %.o: %.c
	$(CC) -fPIC -c $<

libvigra_c.so:
	cp ../../../vigra_c/vigra_c/bin/libvigra_c.so .

.PHONY: clean realclean
clean :
	rm -f *.[oa] *~

realclean: clean
	rm -f $(LIBNAME).so
