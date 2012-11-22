DEBUG=-g

OBJ=limo.o writer.o reader.o error.o makers.o vars.o eval.o \
	helpers.o builtinenv.o builtins.o numbers_gmp.o annotations.o
CC=gcc $(DEBUG) -O3

.PHONY: all clean realclean

limo: $(OBJ)
	gcc $(OBJ) -rdynamic -lgc -lgmp -ldl -lreadline -o limo

all: limo

$(OBJ): limo.h

clean:	
	rm -f *.o *~

realclean: clean
	rm limo
