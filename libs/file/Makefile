LIBNAME=file
OBJ = $(LIBNAME).o 
CFLAGS += -I../..


$(LIBNAME).so: $(OBJ) Makefile
	$(CC) $(CFLAGS) -o $(LIBNAME).so -shared $(OBJ)

$(LIBNAME).a: $(OBJ) Makefile
	$(AR) rcs file.a file.o

file.o: file.c
	$(CC) $(CFLAGS) -fPIC -c file.c

.PHONY: clean
clean :
	rm -f *.[oa] *~

realclean: clean
	rm -f $(LIBNAME).so
