INSTALL_PREFIX=/usr/local
LIMO_PREFIX=$(INSTALL_PREFIX)/lib/limo/

DEBUG=-g
PROFILING=#-pg -no-pie
OPTIMIZE=-O3
OPTIONS=-DLIMO_PREFIX=\"$(LIMO_PREFIX)\"

BASEOBJ=writer.o reader.o error.o makers.o vars.o eval.o \
	helpers.o builtinenv.o builtins.o numbers_gmp.o annotations.o \
	dict.o special.o block.o limpy.o misc.o inlined_mods.o
OBJ=limo.o $(BASEOBJ)
EXEOBJ=exelimo.o $(BASEOBJ)
HEADERS=limo.h config.h

CFLAGS += $(OPTIMIZE) $(DEBUG) $(PROFILING) $(OPTIONS)

.PHONY: all libs clean realclean install uninstall

all: libs limo

exelimo: $(EXEOBJ)
	ar rcs liblimo.a $(EXEOBJ)
exelimo.o: $(HEADERS) limo.c
	$(CC) $(DEBUG) -c -DLIMO_MAKE_EXECUTABLE limo.c -o exelimo.o

inlined_mods.c: inlined.mods
	./make-inlined_mods-c.sh

limo: $(OBJ)
	$(CC) $(OBJ) $(PROFILING) `./inline-cfg.sh` -rdynamic -lgc -lgmp -ldl -lreadline -lm -o limo

limo-almost-static: $(OBJ)
	$(CC) $(OBJ) `./inline-cfg.sh` -lm -l:libgc.a -lpthread -l:libgmp.a -ldl -l:libreadline.a -l:libtermcap.a -rdynamic -o limo

limo-wsl: limo-almost-static

libs:
	make -C libs

TAGS:
	etags *.c *.h libs/*/*.c libs/*/*.h

$(OBJ): $(HEADERS) Makefile

clean:	
	rm -f *.o *~
	rm -f inlined_mods.c
	make -C libs clean

realclean: clean
	make -C libs realclean
	rm -f limo

install:
	mkdir -p $(LIMO_PREFIX)
	rm -r $(LIMO_PREFIX)/*
	cp *.limo $(LIMO_PREFIX)
	cp libs/*/*.so $(LIMO_PREFIX)
	cp limo $(INSTALL_PREFIX)/bin

uninstall:
	rm -rf $(LIMO_PREFIX)
	rm -f $(INSTALL_PREFIX)/bin/limo
