DEBUG=-g

PROFILING= #-pg

OPTIMIZE=-O3

OBJ=limo.o writer.o reader.o error.o makers.o vars.o eval.o \
	helpers.o builtinenv.o builtins.o numbers_gmp.o annotations.o \
	dict.o

CC=gcc $(OPTIMIZE) $(DEBUG) $(PROFILING) 

.PHONY: all libs clean realclean

all: limo libs

limo: $(OBJ)
	gcc $(OBJ) $(PROFILING) -rdynamic -lgc -lgmp -ldl -lreadline -o limo

libs:
	make -C libs

$(OBJ): limo.h Makefile

clean:	
	rm -f *.o *~
	make -C libs clean

realclean: clean
	make -C libs realclean
	rm limo
