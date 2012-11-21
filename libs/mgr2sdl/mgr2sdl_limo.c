/* M_GR2 : Graphikbibliothek f〉 VGA 640 * 480 * 16
 * Adapterbibliothek: mgr2sdl
*/

#include "mgr2sdl.h"
#include <SDL.h>
#include "../../limo.h"

BUILTIN(builtin_mgr_startgraphics)
{
  SetMode(GRAPHIK);
  return make_nil();
}

BUILTIN(builtin_mgr_starttext)
{
  SetMode(TEXT);
  return make_nil();
}

BUILTIN(builtin_mgr_getmode)
{
  switch (GetMode()) {
  case GRAPHIK: return make_sym(":graphik");
  case TEXT:    return make_sym(":text");
  default: return make_nil();
  }
}

BUILTIN(builtin_mgr_enablerefresh) 
{
  EnableRefresh();
  return make_nil();
}

BUILTIN(builtin_mgr_disablerefresh)
{
  DisableRefresh();  // this returns int. don't know why. i didn't tell me.
  return make_nil();
}


//picture CreatePicture(int width, int height);   /**/
//fastpicture CreateFastPicture(int width, int height); /**/

//picture LoadPicture(char []);           /**/
//fastpicture LoadFastPicture(char []);   /**/
//picture LoadIcon(char []);              /**/
//int     SavePicture(picture, char []);  /**/
//int     SaveFastPicture(fastpicture, char []);  /**/
//int     SaveIcon(picture, char []);     /**/

//int     DestroyPicture(picture *);      /**/
//int     DestroyFastPicture(fastpicture *);      /**/

//int     PutPicture(picture, int x, int y);      /**/
//picture GetPicture(int x1, int y1, int x2, int y2);     /**/
//int     PutFastPicture(fastpicture, int x, int y);      /**/
//fastpicture GetFastPicture(int x1, int y1, int x2, int y2);     /**/
//picture     FastPic2Pic(fastpicture);   /**/
//fastpicture Pic2FastPic(picture);       /**/

BUILTIN(builtin_mgr_point)
{

int     Point(int x, int y);            /**/
}
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

	void limo_dll_init(limo_data *env) 
	{
	}
			  

