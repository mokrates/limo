LIBS=string_builtins

.PHONY: clean realclean

$(LIBS):
	make -C $@

clean:
	@- $(foreach X, $(LIBS), make -C $X clean)

realclean:
	@- $(foreach X, $(LIBS), make -C $X realclean)
