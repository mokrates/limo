// This file does mostly provide dummy wrappers for the original slow routines.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL.h>
#include "simplesdl.h"

#define ReadData(f, d, l) fread((d), (l), 1, (f))
#define WriteData(f, d, l) fwrite((d), (l), 1, (f))

#define PIC2FPIC(p)	((fastpicture)p)
#define FPIC2PIC(p)	((picture)p)

static picture copy_picture(picture oldpic)
{
	picture pic;
	
	pic = CreatePicture(oldpic->width, oldpic->height);
	pic->pic_ptr=(char *)malloc(pic->width * pic->height);
	
	pic->height = oldpic->height;
	pic->width  = oldpic->width;
	
	memcpy((void *)pic->pic_ptr, (void *)oldpic->pic_ptr,
	       pic->height*pic->width);
		   
	return pic;
}

fastpicture Pic2FastPic(picture pic)
{
	return PIC2FPIC(copy_picture(pic));
}

picture FastPic2Pic(fastpicture fpic)
{
	return copy_picture(FPIC2PIC(fpic));
}

int PutFastPicture(fastpicture fpic, int ax, int ay)
{
  return PutPicture(FPIC2PIC(fpic), ax, ay);
}

fastpicture GetFastPicture(int x1, int y1, int x2, int y2)
{
	return PIC2FPIC(GetPicture(x1, y1, x2, y2));
}

int DestroyFastPicture(fastpicture *fpic)
{
	return DestroyPicture((picture *)&fpic);
}

fastpicture LoadFastPicture(char fname[])
{
	return PIC2FPIC(LoadPicture(fname));
}

int SaveFastPicture(fastpicture fpic, char fname[])
{
	return SavePicture(FPIC2PIC(fpic), fname);
}

fastpicture CreateFastPicture(int width, int height)
{
	return PIC2FPIC(CreatePicture(width, height));
}
