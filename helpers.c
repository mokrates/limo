#include "limo.h"
#include <string.h>

inline int (is_nil)(limo_data *ld)
{
  return ld->type == limo_TYPE_NIL;
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
  cdr = CDR(x);
  while (cdr->type == limo_TYPE_THUNK)
    cdr = eval(CDR(cdr), CAR(cdr));

  return cdr;
}
