#include "mgr2sdl.h"

#define abs(x) ((x)<0?-(x):(x))

void Line(int x1, int y1, int x2, int y2, int color)
{
  if ((x1-x2)||(y1-y2)) { 
    if (abs(x1-x2)>=abs(y1-y2)) {
      float steigung;
      int x=0;
            
      if (x2<x1) {              /* Koordinaten tauschen */
        int swap;

        swap=x1;
        x1=x2;
        x2=swap;

        swap=y1;
        y1=y2;
        y2=swap;
      }

      steigung=(((float)y2-y1)/(x2-x1));
      while (x<=x2-x1) {
        Plot(x+x1, steigung*x+y1, color);
        x++;
      }
    } 
    else {
      int y=0;
      float steigung;
      
      if (y2<y1) {              /* Koordinaten tauschen */
        int swap;
          
        swap=x1;
        x1=x2;
        x2=swap;
          
        swap=y1;
        y1=y2;
        y2=swap;
      }

      steigung=(((float)x2-x1)/(y2-y1));
      while (y<=y2-y1) {
        Plot(steigung*y+x1, y+y1, color);
        y++;
      }
    } 
  }
}
