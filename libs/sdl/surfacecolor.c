#include "limo_sdl.h"

// create a color-object usable with the given surface
BUILTIN(builtin_sdl_surface_rgbpcolor)
{
  limo_data *ld_surface, *ld_r, *ld_g, *ld_b;
  SDL_Surface *surface;
  REQUIRE_ARGC("SDL-SURFACE-RGBCOLOR", 4);
  unsigned int r,g,b, return_color;

  ld_surface = eval(FIRST_ARG, env);
  ld_r = eval(SECOND_ARG, env);
  ld_g = eval(THIRD_ARG, env);
  ld_b = eval(FOURTH_ARG, env);

  surface = get_special(ld_surface, sym_sdl_surface);
  r = GETINTFROMNUMBER(ld_r);
  g = GETINTFROMNUMBER(ld_g);
  b = GETINTFROMNUMBER(ld_b);

  return make_rational_from_long_long(SDL_MapRGB(surface->format, r,g,b));
}

BUILTIN(builtin_sdl_surface_rgbapcolor)
{
  limo_data *ld_surface, *ld_r, *ld_g, *ld_b, *ld_a;
  SDL_Surface *surface;
  REQUIRE_ARGC("SDL-SURFACE-RGBACOLOR", 5);
  unsigned int r,g,b,a, return_color;

  ld_surface = eval(FIRST_ARG, env);
  ld_r = eval(SECOND_ARG, env);
  ld_g = eval(THIRD_ARG, env);
  ld_b = eval(FOURTH_ARG, env);
  ld_a = eval(FIFTH_ARG, env);

  surface = get_special(ld_surface, sym_sdl_surface);
  r = GETINTFROMNUMBER(ld_r);
  g = GETINTFROMNUMBER(ld_g);
  b = GETINTFROMNUMBER(ld_b);
  a = GETINTFROMNUMBER(ld_a);

  return make_rational_from_long_long(SDL_MapRGBA(surface->format, r,g,b,a));
}

BUILTIN(builtin_sdl_makecolor)
{
  SDL_Color *sc_result;
  REQUIRE_ARGC("SDL-MAKECOLOR", 3);
  int r,g,b;
  limo_data *ld_r, *ld_g, *ld_b;
  
  sc_result = (SDL_Color *)GC_malloc(sizeof (SDL_Color));
  ld_r = eval(FIRST_ARG, env);
  ld_g = eval(SECOND_ARG, env);
  ld_b = eval(THIRD_ARG, env);
  
  sc_result->r = GETINTFROMNUMBER(ld_r);
  sc_result->g = GETINTFROMNUMBER(ld_g);
  sc_result->b = GETINTFROMNUMBER(ld_b);
  return make_special(sym_sdl_color, sc_result);
}
