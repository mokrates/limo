#include "limo_sdl.h"

BUILTIN(builtin_sdl_clearsurface)
{
  limo_data *ld_surface, *ld_color;
  SDL_Surface *surface;
  unsigned int color;
  REQUIRE_ARGC("SDL-CLEARSURFACE", 2);
  ld_surface = eval(FIRST_ARG, env);
  ld_color = eval(SECOND_ARG, env);

  surface = get_special(ld_surface, sym_sdl_surface);
  color = GETINTFROMMPQ(ld_color);

  SDL_FillRect(surface, NULL, color);

  return nil;
}
