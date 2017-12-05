// Hier sind die Lockingfunktionen gewrappt.
#include "mgr2sdl.h"
#include <SDL.h>

static int counter=0;

int DisableRefresh(void)	// lock the surface
{
//	printf("%i\n", counter);
	
	++counter;

	if (counter > 1) return 1;	// nothing to do.

/*    if ( SDL_MUSTLOCK(mgr2sdl_surface) ) {
        if ( SDL_LockSurface(mgr2sdl_surface) < 0 ) {
			counter = 0;	// back to zero, *really* not locked.
            return 0;
        }
    }*/
	
	return 1;
}

void EnableRefresh(void)	// unlock and flip
{
//	printf("%i", counter);

	if (counter > 0)
		--counter;
		
	if (counter > 0)		// don't unlock yet
		return;

	// dislocking
/*	if ( SDL_MUSTLOCK(mgr2sdl_surface) ) {
        SDL_UnlockSurface(mgr2sdl_surface);
    }*/

	// flipping
	SDL_Flip(mgr2sdl_surface);
}
