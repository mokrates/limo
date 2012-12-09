OBJ = string_builtins.o 
CFLAGS +=  -I../.. 

string_builtins.so: $(OBJ) Makefile
	$(CC) -o string_builtins.so -shared $(OBJ)

.PHONY: clean
clean :
	rm -f *.[oa] *~

realclean: clean
	rm -f string_builtins.so
