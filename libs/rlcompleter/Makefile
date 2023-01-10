LIBNAME=rlcompleter
OBJ = $(LIBNAME).o
CFLAGS += -g -I../.. -fPIC

$(LIBNAME).so: $(OBJ) Makefile
	$(CC) $(CFLAGS) -o $(LIBNAME).so -shared $(OBJ)

$(LIBNAME).dll: $(OBJ) Makefile
	$(CC) $(CFLAGS) -o $(LIBNAME).dll -shared  ../../limodll.dll $(OBJ) -lgc -lgmp -lreadline

$(LIBNAME).a: $(OBJ) Makefile
	$(AR) rcs $(LIBNAME).a $(OBJ)

.PHONY: clean
clean :
	rm -f *.[oa] *~

realclean: clean
	rm -f $(LIBNAME).so
