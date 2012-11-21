#include "limo.h"

limo_annotation *make_annotation(void)
{
  limo_annotation *la = (limo_annotation *)GC_malloc(sizeof (limo_annotation));
  return memset(la, 0, sizeof *la);
}

void annotate(limo_data *ld, char *filename, int line)
{
  limo_annotation *la = make_annotation();
  la->filename = filename;
  la->line = line;
}
