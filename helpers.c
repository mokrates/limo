#include "limo.h"
#include <string.h>

int is_nil(limo_data *ld)
{
  if (!ld)
    limo_error("null pointer (is_nil)");
  return ld->type == limo_TYPE_CONS && !ld->data.d_cons;
}

int limo_equals(limo_data *a, limo_data *b)
{
  if (a->type != b->type)
    return 0;

  switch (a->type) {
  case limo_TYPE_SYMBOL:
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
