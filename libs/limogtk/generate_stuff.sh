#!/bin/bash

echo '#include <gtk/gtk.h>' > _gtk-h.c
gcc $(pkg-config gtk+-3.0 --cflags) -E _gtk-h.c -o _gtk-h.E.c

cat _gtk-h.E.c | grep -v '^#' | limo enums.limo | tee _limogtk-enums.h
cat _gtk-h.E.c | grep -v '^#' | limo gen_builtins.limo
