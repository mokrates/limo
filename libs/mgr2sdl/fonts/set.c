#include "../mgr2sdl.h"
#include "fonts.h"

static int FontSize=16;
static int FontColor=15;
static int FontBColorP=0;
static int FontBColor;

static int FontX=0, FontY=0;

void FontSetSize(int x)
{
  FontSize=x;
}

int FontGetSize(void)
{
  return FontSize;
}

void FontSetColor(int f, int b)
{
  FontColor=f;
  if (b==F_TRANSPARENT)
    FontBColorP=0;
  else {
    FontBColorP=1;
    FontBColor=b;
  }
}

int FontGetColor(void)
{
  return FontColor;
}

int FontGetBColor(void)
{
  if (FontBColorP)
    return FontBColor;
  else
    return F_TRANSPARENT;
}


void FontGotoXY(int x, int y)
{
  FontX=x;
  FontY=y;
}

int FontGetX(void)
{
  return FontX;
}

int FontGetY(void)
{
  return FontY;
}
