DEBUG=-g

PROFILING= #-pg

OPTIMIZE=-O3

OBJ=limo.o writer.o reader.o error.o makers.o vars.o eval.o \
	helpers.o builtinenv.o builtins.o numbers_gmp.o annotations.o \
	dict.o

CC=gcc $(OPTIMIZE) $(DEBUG) $(PROFILING) 

.PHONY: all clean realclean

limo: $(OBJ)
	gcc $(OBJ) $(PROFILING) -rdynamic -lgc -lgmp -ldl -lreadline -o limo

all: limo

$(OBJ): limo.h Makefile

clean:	
	rm -f *.o *~

realclean: clean
	rm limo
