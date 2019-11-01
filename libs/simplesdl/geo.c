#include "simplesdl.h"
#include <SDL.h>

void HLine(int x1, int x2, int y, int c)
{
  DisableRefresh();
  while (x1<=x2) {
    Plot(x1, y, c);
    x1++;
  }
  EnableRefresh();
}

void VLine(int x, int y1, int y2, int c)
{
  DisableRefresh();
  while (y1<=y2) {
    Plot(x, y1, c);
    y1++;
  }
  EnableRefresh();
}

void Box(int x1, int y1, int x2, int y2, int c)
{
  DisableRefresh();
  HLine(x1, x2, y1, c);
  HLine(x1, x2, y2, c);
  VLine(x1, y1, y2, c);
  VLine(x2, y1, y2, c);
  EnableRefresh();
}

void FilledBox(int x1, int y1, int x2, int y2, int c)
{
	SDL_Rect r;
	r.x = x1; r.y = y1; r.w = x2-x1; r.h = y2-y1;
	DisableRefresh();
	SDL_FillRect(ssdl_surface, &r, ssdl_colormap[c]);
	EnableRefresh();
}

void FastFilledBox(int x1, int y1, int x2, int y2, int c)
{
	FilledBox(x1, y1, x2+1, y2+1, c);
}

