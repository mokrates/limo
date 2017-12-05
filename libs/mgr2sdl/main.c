#include <SDL.h>
#include "mgr2sdl.h"

static int mode = TEXT;

// Globals
SDL_Surface *mgr2sdl_surface;
Uint32 mgr2sdl_colormap[16];

// Die 16 Textmodusfarben. Irgendwo muessen die ja herkommen :)
static void init_colormap(void)
{
	int i;
	
	for (i=0; i<16; ++i)
		mgr2sdl_colormap[i] =
		  SDL_MapRGB(mgr2sdl_surface->format,
		    ((i>>2)&1)*127*((i&8)?2:1),
			((i>>1)&1)*127*((i&8)?2:1),
			(i&1)*127*((i&8)?2:1));
			
	mgr2sdl_colormap[8] = SDL_MapRGB(mgr2sdl_surface->format, 32, 32, 32);
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
        mgr2sdl_surface = SDL_SetVideoMode(640, 480, 8, SDL_SWSURFACE);
		atexit(SDL_Quit);
		
		init_colormap();
		
		mode = GRAPHIK;
    }
    else 	// mode == TEXT
	;	// nothing to be done (we assume exiting of the prog)
}

void mgr2sdl_toggle_fullscreen(void)
{
	SDL_WM_ToggleFullScreen(mgr2sdl_surface);
}
