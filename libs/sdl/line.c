#include "limo_sdl.h"

#define abs(x) ((x)<0?-(x):(x))

void limo_sdl_line(SDL_Surface *surface, int x1, int y1, int x2, int y2, int color)
{
  if ((x1-x2)||(y1-y2)) { 
    if (abs(x1-x2)>=abs(y1-y2)) {
      float pitch;
      int x=0;
            
      if (x2<x1) {              /* Koordinaten tauschen */
        int swap;
        swap=x1; x1=x2; x2=swap;
        swap=y1; y1=y2; y2=swap;
      }

      pitch=(((float)y2-y1)/(x2-x1));
      while (x<=x2-x1) {
        limo_sdl_plot(surface, x+x1, pitch*x+y1, color);
        x++;
      }
    } 
    else {
      int y=0;
      float pitch;
      
      if (y2<y1) {              /* swap coordinates */
        int swap;
        swap=x1; x1=x2; x2=swap;
	swap=y1; y1=y2; y2=swap;
      }

      pitch=(((float)x2-x1)/(y2-y1));
      while (y<=y2-y1) {
        limo_sdl_plot(surface, pitch*y+x1, y+y1, color);
        y++;
      }
    } 
  }
}

BUILTIN(builtin_sdl_line)
{
  limo_data *ld_surface, *ld_point1, *ld_point2, *ld_color;
  SDL_Surface *surface;
  unsigned int x1, y1, x2, y2, color;
  REQUIRE_ARGC("SDL-LINE", 4);
  ld_surface = eval(FIRST_ARG, env);
  ld_point1 = eval(SECOND_ARG, env);
  ld_point2 = eval(THIRD_ARG, env);
  ld_color = eval(FOURTH_ARG, env);

  surface = get_special(ld_surface, sym_sdl_surface);
  x1 = GETINTFROMNUMBER(CAR(ld_point1));
  y1 = GETINTFROMNUMBER(CDR(ld_point1));
  x2 = GETINTFROMNUMBER(CAR(ld_point2));
  y2 = GETINTFROMNUMBER(CDR(ld_point2));
  color = GETINTFROMNUMBER(ld_color);

  limo_sdl_line(surface, x1, y1, x2, y2, color);

  return nil;
}
