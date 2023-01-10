LIBS=file string_builtins simplesdl threading regex sdl ncurses ncmenu\
	fd rlcompleter date vector socket ssl # limogtk

.PHONY: all clean realclean $(LIBS)

export CC
export AR

all: $(LIBS)

$(LIBS):
	make -C $@ $@.so
	make -C $@ $@.a

$(LIBS:%=%.dll):
	make -C $(@:.dll=) $(@:.dll=.a)
	make -C $(@:.dll=) $@

libs-cygwin: $(LIBS:%=%.dll)

clean:
	$(foreach X,$(LIBS), make -C $X clean ; )

realclean:
	$(foreach X,$(LIBS), make -C $X realclean ; )


