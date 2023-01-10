# -*- makefile -*-
INSTALL_PREFIX=/usr/local
LIMO_PREFIX=$(INSTALL_PREFIX)/lib/limo/

DEBUG=-g -Wall #-DNDEBUG
PROFILING= # -pg -no-pie
OPTIMIZE=-O3
OPTIONS=-DLIMO_PREFIX=\"$(LIMO_PREFIX)\"

BASEOBJ=writer.o reader.o error.o makers.o vars.o eval.o \
	helpers.o builtinenv.o builtins.o numbers_gmp.o annotations.o \
	dict.o special.o block.o limpy.o misc.o inlined_mods.o dynamicvars.o \
	limo.o
OBJ=main.o $(BASEOBJ)
EXEOBJ=exelimo.o $(BASEOBJ)
HEADERS=limo.h config.h

CFLAGS += $(OPTIMIZE) $(DEBUG) $(PROFILING) $(OPTIONS)

.PHONY: all libs clean realclean install uninstall

all: TAGS libs limo

exelimo: $(EXEOBJ)
	ar rcs liblimo.a $(EXEOBJ)
exelimo.o: $(HEADERS) limo.c
	$(CC) $(DEBUG) -c -DLIMO_MAKE_EXECUTABLE limo.c -o exelimo.o

inlined_mods.c: inlined.mods
	./make-inlined_mods-c.sh

limo: $(OBJ) libs info
	$(CC) $(OBJ) $(PROFILING) `./inline-cfg.sh` -rdynamic -lpthread -lgc -lgmp -ldl -lreadline -lm -o limo

limo-almost-static: $(OBJ) libs info
	$(CC) $(OBJ) $(PROFILING) `./inline-cfg.sh` -lm -l:libgc.a -lpthread -l:libgmp.a -ldl -l:libreadline.a -l:libtermcap.a -rdynamic -o limo

limo-wsl: limo-almost-static info

libs:
	make -C libs


############################
## cygwin

limo-cygwin-exe: $(OBJ)
	$(CC) $(BASEOBJ) $(PROFILING) -shared `./inline-cfg.sh` -lm -lgc -lpthread -lgmp -ldl -lreadline -rdynamic -o limodll.dll
	$(CC) main.o $(PROFILING) limodll.dll -lm -lgc -lpthread -lgmp -ldl -lreadline -rdynamic -o limo.exe

limo-cygwin: limo-cygwin-exe libs-cygwin info

libs-cygwin:
	make -C libs libs-cygwin

############################



TAGS:
	etags.emacs --regex='{c}/BUILTIN\(FUN\)?(\([^)]+\)/\2/' *.c *.h libs/*/*.c libs/*/*.h

$(OBJ): $(HEADERS) Makefile

info:
	make -C docs docs

clean:	
	rm -f *.o *~
	rm -f inlined_mods.c
	make -C libs clean
	make -C docs clean

realclean: clean
	make -C libs realclean
	make -C docs realclean
	rm -f limo

install:
	mkdir -p $(LIMO_PREFIX)
	cp -f *.limo $(LIMO_PREFIX)
	cp -f libs/*/*.so $(LIMO_PREFIX) || true
	cp -f libs/*/*.dll $(LIMO_PREFIX) || true
	cp -f libs/*/*.ttf $(LIMO_PREFIX)
	cp -rf limo-code/* $(LIMO_PREFIX)
	cp -f docs/limo-info.info $(LIMO_PREFIX)
	mkdir -p $(INSTALL_PREFIX)/bin
	[ -e limo ] && cp -f limo $(INSTALL_PREFIX)/bin
	[ -e limodll.dll ] && mv $(LIMO_PREFIX)/limodll.dll $(INSTALL_PREFIX)/bin   # yeah, I'm sorry I move this around so much
	[ -e limo.exe ] && cp -f limo.exe $(INSTALL_PREFIX)/bin

uninstall:
	rm -rf $(LIMO_PREFIX)
	rm -f $(INSTALL_PREFIX)/bin/limo
