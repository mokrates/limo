#include "limo_sdl.h"

void limo_sdl_plot(SDL_Surface *surface, unsigned int x, unsigned int y, unsigned int c)
{
  switch (surface->format->BytesPerPixel) {
    
  case 1: { /* vermutlich 8 Bit */
    Uint8 *bufp;
    
    bufp = (Uint8 *)surface->pixels + y*surface->pitch + x;
    *bufp = c;
  }
    break;
    
  case 2: { /* vermutlich 15 Bit oder 16 Bit */
    Uint16 *bufp;
    
    bufp = (Uint16 *)surface->pixels + y*surface->pitch/2 + x;
    *bufp = c;
  }
    break;
    
  case 3: { /* slow 24-bit-mode, don't use */
    Uint8 *bufp;
    
    bufp = (Uint8 *)surface->pixels + y*surface->pitch + x * 3;
    if(SDL_BYTEORDER == SDL_LIL_ENDIAN) {
      bufp[0] = c;
      bufp[1] = c >> 8;
      bufp[2] = c >> 16;
    } else {
      bufp[2] = c;
      bufp[1] = c >> 8;
      bufp[0] = c >> 16;
    }
  }
    break;
    
  case 4: { /* prolly 32 Bit */
    Uint32 *bufp;
    
    bufp = (Uint32 *)surface->pixels + y*surface->pitch/4 + x;
    *bufp = c;
  }
    break;
    
  default:
    throw(make_cons(sym_sdl_error, make_string("limo_sdl_plot: somethings wrong... ")));
  }
}

BUILTIN(builtin_sdl_plot)
{
  limo_data *ld_surface, *ld_point, *ld_color;
  unsigned int color, x, y;

  REQUIRE_ARGC("SDL-PLOT", 3);
  ld_surface = eval(FIRST_ARG, env);
  ld_point = eval(SECOND_ARG, env);
  ld_color = eval(THIRD_ARG, env);

  x = GETINTFROMNUMBER(CAR(ld_point));
  y = GETINTFROMNUMBER(CDR(ld_point));
  color = GETINTFROMNUMBER(ld_color);

  limo_sdl_plot(get_special(ld_surface, sym_sdl_surface), x, y, color);

  return nil;
}
