OBJ = string_builtins.o 
CFLAGS +=  -I../..

string_builtins.so: $(OBJ) Makefile
	$(CC) $(CFLAGS) -o string_builtins.so -shared $(OBJ)

string_builtins.a: $(OBJ) Makefile
	$(AR) rcs string_builtins.a $(OBJ)

string_builtins.o: string_builtins.c
	$(CC) $(CFLAGS) -fPIC -c string_builtins.c

.PHONY: clean
clean :
	rm -f *.[oa] *~

realclean: clean
	rm -f string_builtins.so
