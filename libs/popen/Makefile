LIBNAME=popen
OBJ = $(LIBNAME).o 
CFLAGS += -I../..


$(LIBNAME).so: $(OBJ) Makefile
	$(CC) $(CFLAGS) -o $(LIBNAME).so -shared $(OBJ)

$(LIBNAME).a: $(OBJ) Makefile
	$(AR) rcs popen.a popen.o

popen.o: popen.c
	$(CC) $(CFLAGS) -fPIC -c popen.c

.PHONY: clean
clean :
	rm -f *.[oa] *~

realclean: clean
	rm -f $(LIBNAME).so
