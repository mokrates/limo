#include <stdio.h>
#include <stdlib.h>
#include "simplesdl.h"

#define ReadData(f, d, l) fread((d), (l), 1, (f))
#define WriteData(f, d, l) fwrite((d), (l), 1, (f))

picture LoadIcon(char fname[])
{
  FILE *f;
  picture pic;

  if ((pic=(picture)malloc(sizeof(struct PICTURE)))==NULL)
    return NULL;
  
  if ((pic->pic_ptr=(char *)malloc(1024))==NULL) {
    free(pic);
    return NULL;
  }
  
  if ((f=fopen(fname, "rb"))==NULL) {
    free(pic->pic_ptr);
    free(pic);
    return NULL;
  }

  pic->height = 32;
  pic->width  = 32;     /* Icon */

  ReadData(f, pic->pic_ptr, 1024);

  fclose(f);

  return pic;
}

picture LoadPicture(char fname[])
{
  FILE *f;
  picture pic;
  short int a, b;

  if ((pic=(picture)malloc(sizeof(struct PICTURE)))==NULL)
    return NULL;

  if ((f=fopen(fname, "rb"))==NULL) {
    free(pic);
    return NULL;
  }

  ReadData(f, &a, 2);
  ReadData(f, &b, 2);

  if ((pic->pic_ptr = (char *)malloc(a*b))==NULL) {
    fclose(f);
    free(pic);
    return NULL;
  }

  pic->width=a;
  pic->height=b;
  ReadData(f, pic->pic_ptr, a*b);
  fclose(f);

  return pic;
}

int PutPicture(picture pic, int ax, int ay)
{
  int x, y;  
  unsigned char *pos;

  if (pic==NULL)
    return 0;
	
  DisableRefresh();
	
  pos=pic->pic_ptr;

  for (y=ay; y<pic->height+ay; y++)
    for (x=ax; x<pic->width+ax; x++)
      if (*pos != 0xff) 
        Plot(x, y, *(pos++));
      else
        ++pos;
		
  EnableRefresh();
  
//  FilledBox(ax, ay, ax+32, ay+32, 9);

  return 1;
}

picture GetPicture(int x1, int y1, int x2, int y2)
{
  picture pic;
  int x,y;
  
  if ((pic=(picture)malloc(sizeof(struct PICTURE)))==NULL)
    return NULL;

  pic->width=x2-x1;
  pic->height=y2-y1;

  if ((pic->pic_ptr = (char *)malloc(pic->width * pic->height))==NULL) {
    free(pic);
    return NULL;
  }
  
  for (y=0; y<pic->height; y++)
    for (x=0; x<pic->width; x++)
      pic->pic_ptr[y*pic->width + x]= Point(x+x1, y+y1);

  return pic;
}

int DestroyPicture(picture *pic)
{
  if (*pic==NULL)
    return 0;

  free((*pic)->pic_ptr);
  free(*pic);

  *pic=NULL;

  return 1;
}

int SavePicture(picture pic, char fname[])
{
  FILE *f;
  short int a, b;

  if ((f=fopen(fname, "wb"))==NULL)
    return 0;

  a=pic->width;
  b=pic->height;

  WriteData(f, &a, 2);
  WriteData(f, &b, 2);
  WriteData(f, pic->pic_ptr, a*b);
  fclose(f);

  return 1;
}

int SaveIcon(picture pic, char fname[])
{
  FILE *f;
  short int a;

  if ((f=fopen(fname, "wb"))==NULL)
    return 0;
  
  a=32;

  WriteData(f, &a, 2);
  WriteData(f, &a, 2);
  WriteData(f, pic->pic_ptr, a*a);
  fclose(f);

  return 1;
}

picture CreatePicture(int width, int height)
{
  picture pic;

  pic=(picture)malloc(sizeof(struct PICTURE));
  if (pic==NULL)
    return NULL;

  pic->pic_ptr=(char *)malloc(width*height);
  if (pic->pic_ptr==NULL) {
    free(pic);
    return NULL;
  }

  pic->width=width;
  pic->height=height;

  return pic;
}
