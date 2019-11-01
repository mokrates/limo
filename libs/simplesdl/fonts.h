/* Header for libfonts.a */

#ifndef _SIMPLESDL_FONTS_
#define _SIMPLESDL_

#ifndef _SIMPLESDL_
#error please include "simplesdl.h"
#endif

#define F_TRANSPARENT   0xff

struct FontChar {
  int width;
  unsigned long *bitmap;
}; 

typedef struct FONT {
  struct FontChar pics[256];
  int height;
} *font;

font FontLoad(char *);
void FontDestroy(font);

void FontPutC(font, char);
void FontWrite(font, char *);
void FontWritef(font, char *, ...);
void FontNewLine(void);

void FontSetSize(int);
int  FontGetSize(void);

void FontSetColor(int, int);
int  FontGetBColor(void);
int  FontGetColor(void);

void FontGotoXY(int, int);
int  FontGetX(void);
int  FontGetY(void);

#endif /* _SIMPLESDL_FONTS_ */
