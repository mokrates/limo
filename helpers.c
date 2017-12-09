#include "limo.h"
#include <string.h>

inline int (is_nil)(limo_data *ld)
{
  if (!ld)
    limo_error("null pointer (is_nil)");
  return ld->type == limo_TYPE_CONS && !ld->data.d_cons;
}

inline int limo_equals(limo_data *a, limo_data *b)
{
  if (a==b)
    return 1;

  if (a->type != b->type)
    return 0;

  switch (a->type) {
  case limo_TYPE_SYMBOL:
    if (a->hash == 0 || b->hash == 0)
      return !strcmp(a->data.d_string, b->data.d_string);
    else
      return a->hash == b->hash;

  case limo_TYPE_STRING:
    return !strcmp(a->data.d_string, b->data.d_string);
    
  case limo_TYPE_CONS:
    return a->data.d_cons == b->data.d_cons;

  case limo_TYPE_GMPQ:
    return !mpq_cmp(LIMO_MPQ(a), LIMO_MPQ(b));
  }
    
  return 0;
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
  cdr = (x)->data.d_cons->cdr;
  while (cdr->type == limo_TYPE_THUNK)
    cdr = eval((cdr->data.d_thunk->data.d_cons->cdr), CAR(cdr));

  return cdr;
}
