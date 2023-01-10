OBJ = vector.o 
CFLAGS +=  -I../..


vector.so: $(OBJ) Makefile
	$(CC) $(CFLAGS) -o vector.so -shared $(OBJ)

vector.a: $(OBJ) Makefile
	$(AR) rcs vector.a $(OBJ)

vector.o: vector.c
	$(CC) $(CFLAGS) -fPIC -c vector.c

.PHONY: clean
clean :
	rm -f *.[oa] *~

realclean: clean
	rm -f vector.so
