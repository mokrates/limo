// limo_TYPE_SPECIAL and
// limo_TYPE_SPECIAL_INTERN

#include "limo.h"

// creates a limo_TYPE_SPECIAL and returns it
limo_data *make_special(limo_data *type_symbol, void *content)
{
  limo_data *ld_cons,
    *ld_res = make_limo_data(), 
    *ld_contents = make_limo_data();

  ld_contents->type = limo_TYPE_SPECIAL_INTERN;
  ld_contents->data.d_special_intern = content;
  ld_cons = make_cons(type_symbol, ld_contents);
  ld_res->type = limo_TYPE_SPECIAL;
  ld_res->data.d_special = ld_cons;
  return ld_res;
}

// takes expr, checks, if the type_symbol matches (limo_error if not)
// and returns the contents
void *get_special(limo_data *expr, limo_data *type_symbol)
{
  if (expr->type != limo_TYPE_SPECIAL ||
      !limo_equals(CAR(expr->data.d_special), type_symbol))
    throw(make_cons(make_string("special type expected"), type_symbol));

  return CDR(expr->data.d_special) -> data.d_special_intern;
}

// writer_special: writes a limo_TYPE_SPECIAL
void writer_special(limo_data *expr)
{
  printf("#<special:");
  writer(expr->data.d_special);
  printf(">");
}

void writer_special_intern(limo_data *expr)
{
  printf("#<special_intern:%p>", expr->data.d_special_intern);
}

limo_data *get_special_type_symbol(limo_data *expr)
{
  if (expr->type != limo_TYPE_SPECIAL)
    throw(make_string("special type expected"));
  
  return CAR(expr->data.d_special);
}
