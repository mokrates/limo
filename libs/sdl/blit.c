#include "limo_sdl.h"

BUILTIN(builtin_sdl_blitsurface)
{
  limo_data *ld_srcsurface, *ld_dstsurface, *ld_dstpoint;
  SDL_Rect dstrect;
  SDL_Surface *srcsurface, *dstsurface;
  ld_srcsurface = eval(FIRST_ARG, env);
  ld_dstsurface = eval(SECOND_ARG, env);
  ld_dstpoint   = eval(THIRD_ARG, env);
  dstrect.x = GETINTFROMMPQ(CAR(ld_dstpoint));
  dstrect.y = GETINTFROMMPQ(CDR(ld_dstpoint));
  srcsurface = get_special(ld_srcsurface, sym_sdl_surface);
  dstsurface = get_special(ld_dstsurface, sym_sdl_surface);
  return make_number_from_long_long(SDL_BlitSurface(srcsurface, NULL, dstsurface, &dstrect));
}
