#include <SDL.h>
#include <SDL_endian.h> /* für einen 24-Bit-Modus unabhängig von der Bytereihenfolge */
#include "mgr2sdl.h"

void Plot(int x, int y, int c)
{
	Uint32 color = mgr2sdl_colormap[c];

	if (!DisableRefresh())
		return;
	
	switch (mgr2sdl_surface->format->BytesPerPixel) {

    	case 1: { /* vermutlich 8 Bit */
            Uint8 *bufp;

            bufp = (Uint8 *)mgr2sdl_surface->pixels + y*mgr2sdl_surface->pitch + x;
            *bufp = color;
        }
        break;

        case 2: { /* vermutlich 15 Bit oder 16 Bit */
            Uint16 *bufp;

            bufp = (Uint16 *)mgr2sdl_surface->pixels + y*mgr2sdl_surface->pitch/2 + x;
            *bufp = color;
        }
        break;

        case 3: { /* langsamer 24-Bit-Modus, selten verwendet */
            Uint8 *bufp;

            bufp = (Uint8 *)mgr2sdl_surface->pixels + y*mgr2sdl_surface->pitch + x * 3;
            if(SDL_BYTEORDER == SDL_LIL_ENDIAN) {
                bufp[0] = color;
                bufp[1] = color >> 8;
                bufp[2] = color >> 16;
            } else {
                bufp[2] = color;
                bufp[1] = color >> 8;
                bufp[0] = color >> 16;
            }
        }
        break;

        case 4: { /* vermutlich 32 Bit */
            Uint32 *bufp;

            bufp = (Uint32 *)mgr2sdl_surface->pixels + y*mgr2sdl_surface->pitch/4 + x;
            *bufp = color;
        }
    	break;
	}
	
	EnableRefresh();
}

int Point(int x, int y)
{
/*  register unsigned char i;
  register int ret=0;

  outportb(0x03ce, 0x04);
  for (i=0; i<4; i++) {
    outportb(0x03cf, i);
    ret |= (( *(SCRN_MEM + 80*y + x/8) & (1<<7-(x%8)) ) != 0) << i ;
  }

  return ret;*/
  
  return 0;
}
