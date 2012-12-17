#include "limo.h"

limo_data *make_const(limo_data *name, limo_data *val)
{
  limo_data *res;
  res = make_cons(name, val);
  res->type = limo_TYPE_CONST;
  return val;
}

void writer_const(limo_data *c)
{
  printf("["); writer(CAR(c)); printf("]");
}
