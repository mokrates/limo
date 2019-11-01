#include "../m_gr2.h"
#include "fonts.h"

void main(void)
{
  font f;
  int i;
  
  SetMode(GRAPHIK);

  f=FontLoad("../cfonts/standard.fnt");

  FontSetColor(0, 3);

  for (i=5; i<40; i+=5) {
    FontSetSize(i);
    FontWrite(f, "0123456789");
    FontNewLine();
  }

  getch();
  SetMode(TEXT);
}
