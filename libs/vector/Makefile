OBJ = vector.o 
CFLAGS +=  -I../..
CC=gcc $(CFLAGS)

vector.so: $(OBJ) Makefile
	$(CC) -o vector.so -shared $(OBJ)

vector.a: $(OBJ) Makefile
	ar rcs vector.a $(OBJ)

vector.o: vector.c
	$(CC) -fPIC -c vector.c

.PHONY: clean
clean :
	rm -f *.[oa] *~

realclean: clean
	rm -f vector.so
