LIBNAME=ssl
OBJ = $(LIBNAME).o 
CFLAGS += -I../.. -fPIC


$(LIBNAME).so: $(OBJ) Makefile
	$(CC) $(CFLAGS) -o $(LIBNAME).so -shared $(OBJ) -lssl

$(LIBNAME).dll: $(OBJ) Makefile
	$(CC) $(CFLAGS) -o $(LIBNAME).dll -shared  ../../limodll.dll $(OBJ) -lgc -lgmp -lssl

$(LIBNAME).a: $(OBJ) Makefile
	$(AR) rcs $(LIBNAME).a $(OBJ)

.PHONY: clean
clean :
	rm -f *.[oa] *~

realclean: clean
	rm -f $(LIBNAME).so
