#include "limo.h"
#include <gc/gc.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

/* limo_data *make_limo_data(void) */
/* { */
/*   limo_data *ld = (limo_data *)GC_malloc(sizeof (limo_data)); */
/*   return ld; */
/* } */

limo_data *make_limo_data(void)
{
  limo_data *result;
  void **make_limo_data_next = pk_limo_data_next_get();
  
  if (*make_limo_data_next == NULL)
    *make_limo_data_next = GC_malloc_many(sizeof (limo_data));

  result = (limo_data *)*make_limo_data_next;
  *make_limo_data_next = GC_NEXT(*make_limo_data_next);
  return result;
}

limo_data *make_nil(void)
{
  limo_data *ld = memset(make_limo_data(), 0, sizeof (limo_data));
  ld->type = limo_TYPE_CONS;
  return ld;
}

/* limo_data *make_cons(limo_data *car, limo_data *cdr) */
/* { */
/*   limo_data *ld = make_nil(); */
/*   ld->data.d_cons = (limo_cons *)GC_malloc(sizeof (limo_cons)); */
/*   ld->data.d_cons->car = car; */
/*   ld->data.d_cons->cdr = cdr; */
/*   return ld; */
/* } */

limo_data *make_cons(limo_data *car, limo_data *cdr)
{
  limo_data *ld = make_nil();
  void **make_cons_next = pk_cons_next_get();
  limo_cons *cons;
  
  if (*make_cons_next == NULL)
    *make_cons_next = GC_malloc_many(sizeof (limo_cons));

  cons = (limo_cons *)*make_cons_next;
  *make_cons_next = GC_NEXT(*make_cons_next);

  ld->data.d_cons = cons;
  ld->data.d_cons->car = car;
  ld->data.d_cons->cdr = cdr;
  return ld;
}

unsigned int next_symbol_identity=1;
limo_data *interned_symbols = NULL;

limo_data *make_sym(char *name) // interned // TODO: use dict to speed up.
{
  limo_data *ld;
  limo_data *is;
  char *cp, *cpi;
  
  if (interned_symbols == NULL) {
    interned_symbols = make_nil();
  }

  cp = (char *)GC_malloc_atomic(strlen(name) +1);
  strcpy(cp, name);

  if ((*cp) != '|') {
    cpi = cp;
    while (*cpi) {
      (*cpi) = toupper(*cpi);
      ++cpi;
    }
  }
  
  for (is=interned_symbols; 
       !is_nil(is) && strcmp(cp, CAR(is)->data.d_string); 
       is=CDR(is));

  if (is_nil(is)) {
    ld = make_limo_data();
    ld->type=limo_TYPE_SYMBOL;
    ld->data.d_string = cp;
    ld->hash = next_symbol_identity;
    
    ++next_symbol_identity;
    
    interned_symbols = make_cons(ld, interned_symbols);

    return ld_dup(ld);
  }
  else {   // found interned symbol. return a copy
    return ld_dup(CAR(is));
  }
}

limo_data *make_sym_uninterned(char *name)
{
  limo_data *ld;
  char *cp, *cpi;
  
  cp = (char *)GC_malloc_atomic(strlen(name) +1);
  strcpy(cp, name);

  if ((*cp) != '|') {
    cpi = cp;
    while (*cpi) {
      (*cpi) = toupper(*cpi);
      ++cpi;
    }
  }
  
  ld = make_limo_data();
  ld->type=limo_TYPE_SYMBOL;
  ld->data.d_string = cp;
  ld->hash = 0;
    
  return ld;
}

limo_data *make_builtin(limo_builtin f)
{
  limo_data *ld = make_limo_data();
  ld->type = limo_TYPE_BUILTIN;
  ld->data.d_builtin=f;

  return ld;
}

limo_data *make_thunk(limo_data *expr, limo_data *env)
{
#if LIMO_TAILCALL_OPT
  limo_data *l = make_cons(env, expr);
  limo_data *ea = make_limo_data();
  ea->type=limo_TYPE_THUNK;
  ea->data.d_thunk = l;
  return ea;
#else
  return eval(expr, env);
#endif
}

limo_data *make_dcons(limo_data *car, limo_data *dyncdr, limo_data *env)
{
  return make_cons(car, make_thunk(dyncdr, env));
}

limo_data *make_string(char *msg)
{
  limo_data *str = make_limo_data();
  str->type = limo_TYPE_STRING;
  str->data.d_string = GC_strdup(msg);
  str->hash = strlen(msg);
  return str;
}

limo_data *make_const(limo_data *name, limo_data *ld)
{
  limo_data *res;
  res = make_cons(ld, name);
  res->type = limo_TYPE_CONST;
  return res;
}
