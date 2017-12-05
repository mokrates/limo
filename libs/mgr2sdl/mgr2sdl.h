/* M_GR2 : Graphikbibliothek fÅr VGA 640 * 480 * 16
 * Adapterbibliothek: mgr2sdl
*/

#ifndef _MOLLE_GR2_
#define _MOLLE_GR2_

#include <stdio.h>
#include <SDL.h>

#define GRAPHIK 18
#define TEXT 3

#define XT2G(x) ((x)*8)
#define YT2G(x) ((x)*16)

// #define SCRN_MEM ( (unsigned char *) 0xe00a0000 )

typedef struct PICTURE {
  int width, 
      height;
  unsigned char *pic_ptr;
} *picture, *fastpicture;

/* typedef struct FASTPICTURE {
  int width,
      height;
  unsigned char *(pic_ptr[4]);
} *fastpicture; */

/***** mgr2sdl interna ********/
extern Uint32 mgr2sdl_colormap[16];
extern SDL_Surface *mgr2sdl_surface;

/******************************/

void    SetMode(int mode);              /**/
int     GetMode(void);                  /**/

void    EnableRefresh(void);            /**/
int     DisableRefresh(void);           /**/
// changed interface of DisableRefresh(). type set from void to int

picture CreatePicture(int width, int height);   /**/
fastpicture CreateFastPicture(int width, int height); /**/

picture LoadPicture(char []);           /**/
fastpicture LoadFastPicture(char []);   /**/
picture LoadIcon(char []);              /**/
int     SavePicture(picture, char []);  /**/
int     SaveFastPicture(fastpicture, char []);  /**/
int     SaveIcon(picture, char []);     /**/

int     DestroyPicture(picture *);      /**/
int     DestroyFastPicture(fastpicture *);      /**/

int     PutPicture(picture, int x, int y);      /**/
picture GetPicture(int x1, int y1, int x2, int y2);     /**/
int     PutFastPicture(fastpicture, int x, int y);      /**/
fastpicture GetFastPicture(int x1, int y1, int x2, int y2);     /**/
picture     FastPic2Pic(fastpicture);   /**/
fastpicture Pic2FastPic(picture);       /**/

int     Point(int x, int y);            /**/
void    Plot(int x, int y, int c);      /**/

void    Line(int x1, int y1, int x2, int y2, int c);    /**/
void    HLine(int x1, int x2, int y, int c);            /**/
void    VLine(int x, int y1, int y2, int c);            /**/

void    Box(int x1, int y1, int x2, int y2, int c);     /**/
void    FilledBox(int x1, int y1, int x2, int y2, int c);       /**/
void    FastFilledBox(int x1, int y1, int x2, int y2, int c);   /**/

void    OutText(int c, char fmt[], ...);        /**/

/////// Event Handling

void mgr2sdl_mousefn(int *, int *, int *);
int  mgr2sdl_kbhit(void);
void mgr2sdl_getkey(int *, int *);

/////// misc
void mgr2sdl_toggle_fullscreen(void);


#endif /* _MOLLE_GR2_ */

