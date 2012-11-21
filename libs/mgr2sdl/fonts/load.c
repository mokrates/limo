#include <stdio.h>
#include "../mgr2sdl.h"
#include "fonts.h"

font FontLoad(char *fname)
{
  FILE *f;
  font Font;
  int i, j;

  if ((f=fopen(fname, "rb")) == NULL) 
    return NULL;
  else {
    Font=(font)malloc(sizeof (struct FONT));
    if (Font==NULL)
      return NULL;
    
    fread(&Font->height, sizeof (int), 1, f);
    
    for (i=0; i<256; ++i) {
      fread(&Font->pics[i].width, sizeof (int), 1, f);
      
      Font->pics[i].bitmap = 
        (unsigned long *)calloc(Font->height, sizeof (unsigned long));
                                              
      if (Font->pics[i].bitmap==NULL) {
        while (i-- > 0)        
          free(Font->pics[i].bitmap);
        free(Font);
        return NULL;
      }

      fread(Font->pics[i].bitmap, sizeof (unsigned long), Font->height, f);

    }
  }
  return Font;
}
