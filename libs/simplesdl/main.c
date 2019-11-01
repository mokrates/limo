#include <SDL.h>
#include "simplesdl.h"

static int mode = TEXT;

// Globals
SDL_Surface *ssdl_surface;
Uint32 ssdl_colormap[16];

// Die 16 Textmodusfarben. Irgendwo muessen die ja herkommen :)
static void init_colormap(void)
{
	int i;
	
	for (i=0; i<16; ++i)
		ssdl_colormap[i] =
		  SDL_MapRGB(ssdl_surface->format,
		    ((i>>2)&1)*127*((i&8)?2:1),
			((i>>1)&1)*127*((i&8)?2:1),
			(i&1)*127*((i&8)?2:1));
			
	ssdl_colormap[8] = SDL_MapRGB(ssdl_surface->format, 32, 32, 32);
}

int GetMode(void)	// doesn't work
{
  return mode;
}

void SetMode(int newmode)	// SetMode(GRAPHIK initializes SDL)
{
    if (newmode == GRAPHIK) {
		if (mode == GRAPHIK) {
			DisableRefresh();
			FilledBox(0, 0, 639, 479, 0);
			EnableRefresh();
			return;
		}
		SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
        ssdl_surface = SDL_SetVideoMode(640, 480, 8, SDL_SWSURFACE);
		atexit(SDL_Quit);
		
		init_colormap();
		
		mode = GRAPHIK;
    }
    else 	// mode == TEXT
	;	// nothing to be done (we assume exiting of the prog)
}

void ssdl_toggle_fullscreen(void)
{
	SDL_WM_ToggleFullScreen(ssdl_surface);
}
