#include <limo.h>
#include <curses.h>

#define INS_SDL_BUILTIN(f, name) setq(limo_sdl_env, make_sym(name), make_builtin(f))
extern limo_data *sym_ncurses_window;
