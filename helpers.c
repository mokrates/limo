#include "limo.h"
#include <string.h>

inline int (is_nil)(limo_data *ld)
{
  if (!ld)
    limo_error("null pointer (is_nil)");
  return ld->type == limo_TYPE_CONS && !ld->d_cons;
}

int list_length(limo_data *l)
{
  int i = 0;
  while (!is_nil(l) && l->type == limo_TYPE_CONS) {
    ++i;
    l=CDR(l);
  }
  return i;
}

char *limo_strdup(char *in)
{
  char *newstr = (char *)GC_malloc(strlen(in)+1);
  strcpy(newstr, in);
  return newstr;
}

limo_data *thunk_safe_cdr(limo_data *x)
{
  limo_data *cdr;
  cdr = (x)->d_cons->cdr;
  while (cdr->type == limo_TYPE_THUNK)
    cdr = eval(CDR(cdr), CAR(cdr));

  return cdr;
}
