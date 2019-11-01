#include <stdarg.h>
#include <dos.h>
#include "simplesdl.h"

void OutText(int c, char fmt[], ...)
{
  va_list vl;
  char text[512];
  int i;
  union REGS regs;
  
  va_start(vl, fmt);
  vsprintf(text, fmt, vl);
  va_end(vl);
 
  for (i=0; text[i]!='\0'; i++) {
    regs.h.ah=0x0e;
    regs.h.al=text[i];
    regs.h.bl=c;
    int86(0x10, &regs, &regs);
  }
}
