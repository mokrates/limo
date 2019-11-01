// Hier sind die Lockingfunktionen gewrappt.
#include "simplesdl.h"
#include <SDL.h>

static int counter=0;

int DisableRefresh(void)	// lock the surface
{
//	printf("%i\n", counter);
	
	++counter;

	if (counter > 1) return 1;	// nothing to do.

/*    if ( SDL_MUSTLOCK(ssdl_surface) ) {
        if ( SDL_LockSurface(ssdl_surface) < 0 ) {
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
/*	if ( SDL_MUSTLOCK(ssdl_surface) ) {
        SDL_UnlockSurface(ssdl_surface);
    }*/

	// flipping
	SDL_Flip(ssdl_surface);
}
