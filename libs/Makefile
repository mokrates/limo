LIBS=file string_builtins cvector vigra_limo mgr2sdl

.PHONY: all clean realclean $(LIBS)

all: $(LIBS)

$(LIBS):
	make -C $@

clean:
	$(foreach X,$(LIBS), make -C $X clean ; )

realclean:
	$(foreach X,$(LIBS), make -C $X realclean ; )
