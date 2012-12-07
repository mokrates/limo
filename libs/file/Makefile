LIBNAME=file
OBJ = $(LIBNAME).o 
CC = gcc -I../.. 

$(LIBNAME).so: $(OBJ) Makefile
	gcc -o $(LIBNAME).so -shared $(OBJ)

.PHONY: clean
clean :
	rm -f *.[oa] *~

realclean: clean
	rm -f $(LIBNAME).so
