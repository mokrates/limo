LIBNAME=newt
OBJ = $(LIBNAME).o
CFLAGS += `pkg-config libnewt --cflags` -Wall -g -I../.. -fPIC 

$(LIBNAME).so: $(OBJ) Makefile
	$(CC) $(CFLAGS) -o $(LIBNAME).so -shared $(OBJ) `pkg-config libnewt --libs`

$(LIBNAME).dll: $(OBJ) Makefile
	$(CC) $(CFLAGS) -o $(LIBNAME).dll -shared  ../../limodll.dll $(OBJ) -lgc -lgmp `pkg-config libnewt --libs`

$(LIBNAME).a: $(OBJ) Makefile
	$(AR) rcs $(LIBNAME).a $(OBJ) `pkg-config libnewt --static`

.PHONY: clean
clean :
	rm -f *.[oa] *~

realclean: clean
	rm -f $(LIBNAME).so
