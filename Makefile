INSTALL_PREFIX=/usr/local
LIMO_PREFIX=$(INSTALL_PREFIX)/lib/limo/

DEBUG=-g
PROFILING= #-pg
OPTIMIZE=-O3
OPTIONS=-DLIMO_PREFIX=\"$(LIMO_PREFIX)\"

OBJ=limo.o writer.o reader.o error.o makers.o vars.o eval.o \
	helpers.o builtinenv.o builtins.o numbers_gmp.o annotations.o \
	dict.o special.o limpy.o
HEADERS=limo.h config.h

CFLAGS += $(OPTIMIZE) $(DEBUG) $(PROFILING) $(OPTIONS)

.PHONY: all libs clean realclean install uninstall

all: limo libs

limo: $(OBJ)
	$(CC) $(OBJ) $(PROFILING) -rdynamic -lgc -lgmp -ldl -lreadline -o limo

libs:
	make -C libs

$(OBJ): $(HEADERS) Makefile

clean:	
	rm -f *.o *~
	make -C libs clean

realclean: clean
	make -C libs realclean
	rm -f limo

install:
	mkdir -p $(LIMO_PREFIX)
	cp *.limo $(LIMO_PREFIX)
	cp libs/*/*.so $(LIMO_PREFIX)
	cp limo $(INSTALL_PREFIX)/bin

uninstall:
	rm -rf $(LIMO_PREFIX)
	rm -f $(INSTALL_PREFIX)/bin/limo
