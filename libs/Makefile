LIBS=file string_builtins simplesdl threading regex

.PHONY: all clean realclean $(LIBS)

all: $(LIBS)

$(LIBS):
	make -C $@ $@.so
	make -C $@ $@.a

clean:
	$(foreach X,$(LIBS), make -C $X clean ; )

realclean:
	$(foreach X,$(LIBS), make -C $X realclean ; )
