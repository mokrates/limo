OBJ = string_builtins.o 
CC = gcc -I../.. 

string_builtins.so: $(OBJ) Makefile
	gcc -o string_builtins.so -shared $(OBJ)

.PHONY: clean
clean :
	rm -f *.[oa] *~

realclean: clean
	rm string_builtins.so
