LIBS=file string_builtins threading regex ncurses ncmenu mhash\
	fd rlcompleter date vector socket ssl popen speedup  # limogtk

.PHONY: all clean realclean $(LIBS)

export CC
export AR

all: $(LIBS)

$(LIBS):
	make -C $@ $@.so
	make -C $@ $@.a

##########################
## needed for cygwin
#########################

$(LIBS:%=%.a):
	make -C $(@:.a=) $@

$(LIBS:%=%.dll):
	make -C $(@:.dll=) $(@:.dll=.a)
	make -C $(@:.dll=) $@

libs-cygwin-dll: $(LIBS:%=%.dll)
libs-cygwin-a: $(LIBS:%=%.a)

## !cygwin

clean:
	$(foreach X,$(LIBS), make -C $X clean ; )

realclean:
	$(foreach X,$(LIBS), make -C $X realclean ; )


