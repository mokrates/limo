#define GC_THREADS
#include <limo.h>
#include <SDL.h>
#include <SDL_ttf.h>

#define INS_SDL_BUILTIN(f, name) setq(limo_sdl_env, make_sym(name), make_builtin(f, name))

void limo_sdl_line(SDL_Surface *, int, int, int, int, int);
void limo_sdl_plot(SDL_Surface *, unsigned int, unsigned int, unsigned int);

extern limo_data *sym_sdl_error;
extern limo_data *sym_sdl_surface;
extern limo_data *sym_sdl_color;
extern limo_data *sym_sdl_font;
extern limo_data *sym_sdl_event;

BUILTIN(builtin_sdl_setvideomode);
BUILTIN(builtin_sdl_surface_rgbpcolor);
BUILTIN(builtin_sdl_surface_rgbapcolor);
BUILTIN(builtin_sdl_makecolor);
BUILTIN(builtin_sdl_plot);
BUILTIN(builtin_sdl_line);
BUILTIN(builtin_sdl_clearsurface);

BUILTIN(builtin_sdl_ttf_init);
BUILTIN(builtin_sdl_ttf_quit);
BUILTIN(builtin_sdl_ttf_wasinit);
BUILTIN(builtin_sdl_ttf_openfont);
BUILTIN(builtin_sdl_ttf_sizetext);
BUILTIN(builtin_sdl_ttf_rendertext_blended);

BUILTIN(builtin_sdl_blitsurface);
