/* M_GR2 : Graphikbibliothek f〉 VGA 640 * 480 * 16
 * Adapterbibliothek: mgr2sdl
*/

#include "mgr2sdl.h"
#include <SDL.h>
#include <limo.h>

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

/// this has to be fixed... this should return color at point...
/* BUILTIN(builtin_mgr_point) */
/* { */
/*   limo_data *x, *y; */
/*   if (list_length(arglist) < 3) */
/*     limo_error("(point X Y)"); */
  
/*   x=eval(FIRST_ARG, env); */
/*   y=eval(SECOND_ARG, env); */
  
/*   Point(mpq_get_d(*x->data.d_mpq), mpq_get_d(*y->data.d_mpq)); */

/*   return make_nil(); */
/* } */

BUILTIN(builtin_mgr_plot)
{
  limo_data *x, *y, *c;
  if (list_length(arglist) < 4)
    limo_error("(plot X Y COLOR)");
  
  x=eval(FIRST_ARG, env);
  y=eval(SECOND_ARG, env);
  c=eval(THIRD_ARG, env);
  
  Plot(mpq_get_d(*x->data.d_mpq), 
       mpq_get_d(*y->data.d_mpq),
       mpq_get_d(*c->data.d_mpq));

  return make_nil();
}

#define SHIFT_ARGS() (arglist = CDR(arglist))

BUILTIN(builtin_mgr_line)
{
  limo_data *x1, *y1, *x2, *y2, *c;
  if (list_length(arglist) < 6)
    limo_error("(line X1 Y1 X2 Y2 COLOR)");
  
  x1=eval(FIRST_ARG, env); SHIFT_ARGS();
  y1=eval(FIRST_ARG, env); SHIFT_ARGS();
  x2=eval(FIRST_ARG, env); SHIFT_ARGS();
  y2=eval(FIRST_ARG, env); SHIFT_ARGS();
  c=eval(FIRST_ARG, env);
  
  Line(mpq_get_d(*x1->data.d_mpq), 
       mpq_get_d(*y1->data.d_mpq),
       mpq_get_d(*x2->data.d_mpq),
       mpq_get_d(*y2->data.d_mpq),
       mpq_get_d(*c->data.d_mpq));

  return make_nil();
}

BUILTIN(builtin_mgr_hline)
{
  limo_data *x1, *x2, *y, *c;
  if (list_length(arglist) < 5)
    limo_error("(line X1 X2 Y COLOR)");
  
  x1=eval(FIRST_ARG, env); SHIFT_ARGS();
  x2=eval(FIRST_ARG, env); SHIFT_ARGS();
  y=eval(FIRST_ARG, env); SHIFT_ARGS();
  c=eval(FIRST_ARG, env);
  
  HLine(mpq_get_d(*x1->data.d_mpq), 
	mpq_get_d(*x2->data.d_mpq),
	mpq_get_d(*y->data.d_mpq),
	mpq_get_d(*c->data.d_mpq));

  return make_nil();
}

BUILTIN(builtin_mgr_vline)
{
  limo_data *x, *y1, *y2, *c;
  if (list_length(arglist) < 5)
    limo_error("(line X Y1 Y2 COLOR)");
  
  x=eval(FIRST_ARG, env); SHIFT_ARGS();
  y1=eval(FIRST_ARG, env); SHIFT_ARGS();
  y2=eval(FIRST_ARG, env); SHIFT_ARGS();
  c=eval(FIRST_ARG, env);
  
  VLine(mpq_get_d(*x->data.d_mpq), 
	mpq_get_d(*y1->data.d_mpq),
	mpq_get_d(*y2->data.d_mpq),
	mpq_get_d(*c->data.d_mpq));

  return make_nil();
}

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

#define INSBUILTIN(f, name) setq(env, make_sym(name), make_builtin(f))

void limo_dll_init(limo_data *env) 
{
  INSBUILTIN(builtin_mgr_startgraphics, "MGR-STARTGRAPHICS");
  INSBUILTIN(builtin_mgr_starttext,     "MGR-STARTTEXT");
  INSBUILTIN(builtin_mgr_getmode,       "MGR-GETMODE");
  INSBUILTIN(builtin_mgr_enablerefresh, "MGR-ENABLEREFRESH");
  INSBUILTIN(builtin_mgr_disablerefresh,"MGR-DISABLEREFRESH");
  INSBUILTIN(builtin_mgr_plot,          "MGR-PLOT");
  INSBUILTIN(builtin_mgr_line,          "MGR-LINE");
  INSBUILTIN(builtin_mgr_hline,         "MGR-HLINE");
  INSBUILTIN(builtin_mgr_vline,         "MGR-VLINE");
}
