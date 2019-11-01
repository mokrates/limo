// This is for the event-loops in 32edit and thinkgr

#include <SDL.h>

static unsigned int mousebtns;
static unsigned int mouse_x, mouse_y;

static unsigned int kbhit;
static unsigned int kb_special;
static unsigned int kb_ascii;

static void update_status(void)
{
	SDL_Event event; /* Event structure */
	mousebtns = SDL_GetMouseState(&mouse_x, &mouse_y);

	if (kbhit) return; // pressed key must be polled

	while(SDL_PollEvent(&event)) {  /* Loop until there are no events left on the queue */
		switch(event.type) {  /* Process the appropiate event type */
    		case SDL_KEYDOWN:  /* Handle a KEYDOWN event */
				kbhit = 1;
				if (event.key.keysym.sym < 256) {
					kb_special = 0;
					kb_ascii = event.key.keysym.sym;
				}
				else {
					kb_special = 1;
					switch (event.key.keysym.sym) {
						case SDLK_UP: kb_ascii = 72; break;
					    case SDLK_DOWN: kb_ascii = 80; break;
        				case SDLK_RIGHT: kb_ascii = 77; break;
						case SDLK_LEFT: kb_ascii = 75; break;
        				case SDLK_PAGEUP: kb_ascii = 73; break;
        				case SDLK_PAGEDOWN: kb_ascii = 81; break;
						default:
							kbhit = 0;		
					}
				}
				break;
		 
			default: /* unhandled event */
				break;
		}
	}
}

void ssdl_mousefn(int *x, int *y, int *btns)	// mouse status
{
	update_status();
	*btns = mousebtns; *x = mouse_x; *y = mouse_y;
}

// was a key pressed?
int ssdl_kbhit(void)	
{
	update_status();
	return kbhit;	
}

// if a key was pressed: which one?
void ssdl_getkey(int *special, int *ascii)	
{
	kbhit=0;
	*special = kb_special;
	*ascii = kb_ascii;
}
