OBJ = string_builtins.o 
CFLAGS +=  -I../..
CC=gcc $(CFLAGS)

string_builtins.so: $(OBJ) Makefile
	$(CC) -o string_builtins.so -shared $(OBJ)

string_builtins.a: string_builtins.c Makefile
	$(CC) -DLIMO_MOD_INLINE -c string_builtins.c
	ar rcs string_builtins.a $(OBJ)

string_builtins.o: string_builtins.c
	$(CC) -fPIC -c string_builtins.c

.PHONY: clean
clean :
	rm -f *.[oa] *~

realclean: clean
	rm -f string_builtins.so
