#include "../mgr2sdl.h"
#include "fonts.h"

static void FontPutC_bigger(font f, char c)
{
  int p, x, y, pos_x, pos_y, size, ewidth, color, bcolor;
  size=FontGetSize();
  pos_x=FontGetX();
  pos_y=FontGetY();
  color=FontGetColor();
  bcolor=FontGetBColor();
  
  ewidth = size * f->pics[c].width / f->height;
  
  if (bcolor!=F_TRANSPARENT)
    FilledBox(pos_x, pos_y, pos_x+ewidth, pos_y+size, bcolor);
  
  for (y=0; y<size; ++y)
    for (x=0; x<ewidth; ++x) {
      p = (f->pics[c].bitmap[y*f->height/size] 
           >> x*f->pics[c].width / ewidth) & 1;
      if (p)
        Plot(x+pos_x, y+pos_y, color);
    }
  
  FontGotoXY(pos_x + ewidth, pos_y);
}

static void FontPutC_smaller(font f, char c)
{
  int p, x, y, pos_x, pos_y, size, ewidth, color, bcolor;
  size=FontGetSize();
  pos_x=FontGetX();
  pos_y=FontGetY();
  color=FontGetColor();
  bcolor=FontGetBColor();
  
  ewidth = size * f->pics[c].width / f->height;
  
  if (bcolor!=F_TRANSPARENT)
    FilledBox(pos_x, pos_y, pos_x+ewidth, pos_y+size, bcolor);
  
  for (y=0; y<f->height; ++y)
    for (x=0; x<f->pics[c].width; ++x) {
      p = (f->pics[c].bitmap[y] >> x) & 1;
      if (p)
        Plot(pos_x + x * ewidth / f->pics[c].width, 
             pos_y + y * size / f->height, 
             color);
    }
  FontGotoXY(pos_x + ewidth, pos_y);
}


void FontPutC(font f, char c)
{
  if (FontGetSize() > f->height)
    FontPutC_bigger(f, c);
  else
    FontPutC_smaller(f, c);
}

void FontWrite(font f, char *t)
{
  while (*t)
    FontPutC(f, *(t++));
}

void FontNewLine(void)
{
  FontGotoXY(0, FontGetY() + FontGetSize());
}
