#include "limo.h"

limo_annotation *make_annotation(char *filename, int line, int col)
{
  limo_annotation *la = (limo_annotation *)GC_malloc(sizeof (limo_annotation));
  memset(la, 0, sizeof *la);

  la->filename = filename;
  la->line = line;
  la->col = col;

  return la;
}

limo_data *get_annotation(limo_data *ld)
{
  char buf[256];
  limo_annotation *la = ld->annotation;

  if (ld->annotation == NULL)
    return make_string("** unknown **");
  else {
    snprintf(buf, 255, "file: %s, line: %i, col: %i", la->filename, la->line+1, la->col);
    return make_string(buf);
  }
}
