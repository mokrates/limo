LIBS=file string_builtins simplesdl threading regex sdl ncurses ncmenu\
	fd popen socket ssl rlcompleter date vector

.PHONY: all clean realclean $(LIBS)

all: $(LIBS)

$(LIBS):
	make -C $@ $@.so
	make -C $@ $@.a

clean:
	$(foreach X,$(LIBS), make -C $X clean ; )

realclean:
	$(foreach X,$(LIBS), make -C $X realclean ; )
