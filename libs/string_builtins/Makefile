OBJ = string_builtins.o 
CFLAGS +=  -I../..
CC=gcc $(CFLAGS)

string_builtins.so: $(OBJ) Makefile
	$(CC) -o string_builtins.so -shared $(OBJ)

string_builtins.o: string_builtins.c
	$(CC) -fPIC -c string_builtins.c

.PHONY: clean
clean :
	rm -f *.[oa] *~

realclean: clean
	rm -f string_builtins.so
