#include "limo.h"
#include <gc.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

/* limo_data *make_limo_data(void) */
/* { */
/*   limo_data *ld = (limo_data *)GC_malloc(sizeof (limo_data)); */
/*   return ld; */
/* } */

void free_limo_data(limo_data *ld)
{
  void **make_limo_data_next = pk_limo_data_next_get();
  
  *((void **)ld) = *make_limo_data_next;
  *make_limo_data_next = ld;
}

limo_data *make_limo_data(void)
{
  limo_data *result;
  void **make_limo_data_next = pk_limo_data_next_get();
  
  if (!*make_limo_data_next)
    *make_limo_data_next = GC_malloc_many(sizeof (limo_data));

  result = (limo_data *)*make_limo_data_next;
  *make_limo_data_next = GC_NEXT(*make_limo_data_next);
  return result;
}

limo_data *make_nil(void)
{
  limo_data *ld = make_limo_data();
  ld->type = limo_TYPE_NIL;
  return ld;
}

limo_data *make_cons(limo_data *car, limo_data *cdr)
{
  limo_data *ld = make_limo_data();
  ld->type = limo_TYPE_CONS;
  CAR(ld) = car;
  CDR(ld) = cdr;
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
       !is_nil(is) && strcmp(cp, CAR(is)->d_string); 
       is=CDR(is));

  if (is_nil(is)) {
    ld = make_limo_data();
    ld->type=limo_TYPE_SYMBOL;
    ld->d_string = cp;
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
  ld->d_string = cp;
  ld->hash = 0;
    
  return ld;
}

limo_data *make_builtin(limo_builtin f, char *name)
{
  limo_data *ld = make_limo_data();
  ld->type = limo_TYPE_BUILTIN;
  ld->d_builtin=f;
  ld->builtin_name = name;

  return ld;
}

limo_data *make_builtinfun(limo_builtinfun f, char *name)
{
  limo_data *ld = make_limo_data();
  ld->type = limo_TYPE_BUILTINFUN;
  ld->d_builtinfun=f;
  ld->builtin_name = name;  

  return ld;
}

limo_data *make_thunk(limo_data *expr, limo_data *env)
{
  // TODO: This #if may be incompatible with THREAD-CLEANUP-PUSH
#if LIMO_TAILCALL_OPT
  limo_data *l = make_cons(env, expr);
  l->type=limo_TYPE_THUNK;
  return l;
#else
  return eval(expr, env);
#endif
}

limo_data *make_dcons(limo_data *car, limo_data *dyncdr, limo_data *env)
{
  return make_cons(car, make_thunk(dyncdr, env));
}

limo_data *make_string(const char *msg)
{
  limo_data *str = make_limo_data();
  str->type = limo_TYPE_STRING;
  str->d_string = GC_strdup(msg);
  str->hash = strlen(msg);
  return str;
}

// create a one byte string without requiring an extra buffer for the string
limo_data *make_char(char c)
{
  limo_data *str = make_limo_data();
  str->type = limo_TYPE_STRING;
  str->d_string = (char *)&(str->cdr);
  str->d_string[1] = '\0';
  str->d_string[0] = c;
  str->string_length = 1;
  return str;
}

limo_data *make_float_from_str(const char *msg)
{
  limo_data *f = make_limo_data();
  f->type = limo_TYPE_DOUBLE;
  sscanf(msg, "%lf", &f->d_double);
  return f;
}

limo_data *make_const(limo_data *name, limo_data *ld)
{
  limo_data *res;
  res = make_cons(ld, name);
  res->type = limo_TYPE_CONST;
  return res;
}
