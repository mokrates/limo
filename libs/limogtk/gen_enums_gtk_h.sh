echo '#include <gtk/gtk.h>' > gtk-h.c
gcc $(pkg-config gtk+-3.0 --cflags) -E gtk-h.c -o gtk-h.E.c

cat gtk-h.E.c | grep -v '^#' | limo enums.limo | tee limogtk-enums.h
