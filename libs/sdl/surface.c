#include "limo_sdl.h"

BUILTIN(builtin_sdl_setvideomode)
{
  limo_data *ld_flags, *ld_width, *ld_height, *ld_bpp;
  int flags, width, height, bpp;
  SDL_Surface *surface;
  
  REQUIRE_ARGC("SDL-SETVIDEOMODE", 4);
  
  ld_width = eval(FIRST_ARG, env);
  ld_height = eval(SECOND_ARG, env);
  ld_bpp = eval(THIRD_ARG, env);
  ld_flags = eval(FOURTH_ARG, env);
  
  flags = GETINTFROMMPQ(ld_flags);
  width = GETINTFROMMPQ(ld_width);
  height = GETINTFROMMPQ(ld_height);
  bpp = GETINTFROMMPQ(ld_bpp);
  flags = GETINTFROMMPQ(ld_flags);
  
  surface = SDL_SetVideoMode(width, height, bpp, flags);
  if (surface == NULL)		    
    throw(make_cons(sym_sdl_error, make_string("could not set video mode:")));
  
  return make_special(sym_sdl_surface, (void *)surface);
}

