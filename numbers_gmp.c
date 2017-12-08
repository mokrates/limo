#include "limo.h"

BUILTIN(builtin_numberp)
{
  if (eval(FIRST_ARG, env)->type == limo_TYPE_GMPQ)
    return sym_true;
  else
    return nil;
}

BUILTIN(builtin_reprn)
{
  limo_data *res=make_nil();
  res->type = limo_TYPE_STRING;
  res->data.d_string = repr_number(eval(FIRST_ARG, env));
  return res;
}

BUILTIN(builtin_idivmod)
{
  limo_data *q = make_number();
  limo_data *r = make_number();
  limo_data *res = make_cons(q, make_cons(r, nil));

  if (list_length(arglist) != 3)
    limo_error("idivmod needs 2 args");

  limo_data *n = eval(FIRST_ARG, env);
  limo_data *d = eval(SECOND_ARG, env);

  if (!mpz_cmp_si(mpq_numref(LIMO_MPQ(d)), 0))
    limo_error("integer division by zero");
  
  mpz_tdiv_qr(mpq_numref(LIMO_MPQ(q)),
	      mpq_numref(LIMO_MPQ(r)),
	      mpq_numref(LIMO_MPQ(n)),
	      mpq_numref(LIMO_MPQ(d)));
  return res;
}

#define CALC2_BUILTIN(fun) BUILTIN(builtin_##fun ) \
{ \
  limo_data *res = make_number(); \
  if (list_length(arglist) != 3) \
    limo_error(#fun " needs 2 args!"); \
\
  limo_data *first = eval(FIRST_ARG, env); \
  limo_data *second = eval(SECOND_ARG, env); \
  REQUIRE_TYPE(#fun, first, limo_TYPE_GMPQ); \
  REQUIRE_TYPE(#fun, second, limo_TYPE_GMPQ); \
  fun(LIMO_MPQ(res), LIMO_MPQ(eval(FIRST_ARG, env)), LIMO_MPQ(eval(SECOND_ARG, env))); \
  return res; \
}

CALC2_BUILTIN(mpq_add);
CALC2_BUILTIN(mpq_sub);
CALC2_BUILTIN(mpq_mul);
CALC2_BUILTIN(mpq_div);

#define CALC1_BUILTIN(fun) BUILTIN(builtin_##fun ) \
{ \
  limo_data *res = make_number(); \
  if (list_length(arglist) != 2) \
    limo_error(#fun " needs 1 arg!"); \
  fun(LIMO_MPQ(res), LIMO_MPQ(eval(FIRST_ARG, env))); \
  return res; \
}

CALC1_BUILTIN(mpq_neg)
CALC1_BUILTIN(mpq_abs)
CALC1_BUILTIN(mpq_inv)

BUILTIN(builtin_ltn)
{
  if (mpq_cmp(LIMO_MPQ(eval(FIRST_ARG, env)), LIMO_MPQ(eval(SECOND_ARG, env))) < 0)
    return sym_true;
  else
    return nil;
}

BUILTIN(builtin_gtn)
{
  if (mpq_cmp(LIMO_MPQ(eval(FIRST_ARG, env)), LIMO_MPQ(eval(SECOND_ARG, env))) > 0)
    return sym_true;
  else
    return nil;
}

limo_data *make_number(void)
{
  limo_data *res = make_nil();
  res->type=limo_TYPE_GMPQ;
  res->data.d_mpq = (mpq_t *)GC_malloc(sizeof (mpq_t));

  mpq_init(LIMO_MPQ(res));
  return res;
}

char *repr_number(limo_data *ld)
{
  return mpq_get_str(NULL, 10, LIMO_MPQ(ld));
}

limo_data *make_number_from_str(char *str)
{
  limo_data *res = make_number();

  mpq_set_str(LIMO_MPQ(res), str, 10);
  mpq_canonicalize(LIMO_MPQ(res));
  return res;
}

limo_data *make_number_from_long_long(long long i)
{
  limo_data *res = make_number();

  mpq_set_d(LIMO_MPQ(res), (double)i);
  mpq_canonicalize(LIMO_MPQ(res));
  return res;
}

limo_data *make_number_from_double(double d)
{
  limo_data *res = make_number();

  mpq_set_d(LIMO_MPQ(res), (double)d);
  mpq_canonicalize(LIMO_MPQ(res));
  return res;
}

struct { char *name; limo_builtin f; } number_builtin_array[] = {
  { "NUMBERP", builtin_numberp },
  { "REPRN", builtin_reprn },
  { "LTN", builtin_ltn },
  { "GTN", builtin_gtn },
  { "MPQ_NEG", builtin_mpq_neg },
  { "MPQ_ABS", builtin_mpq_abs },
  { "MPQ_INV", builtin_mpq_inv },
  { "MPQ_ADD", builtin_mpq_add },
  { "MPQ_SUB", builtin_mpq_sub },
  { "MPQ_MUL", builtin_mpq_mul },
  { "MPQ_DIV", builtin_mpq_div },
  { "IDIVMOD", builtin_idivmod },
};

// needed for gmp, because it uses a
// nonstandard realloc-signature and free-signature (wtf?)
void *limo_gmp_gc_realloc(void *oldptr, size_t oldsize, size_t newsize)
{
  return GC_realloc(oldptr, newsize);
}
void limo_gmp_gc_free(void *ptr, size_t size) 
{ 
  GC_free(ptr);
}

void number_builtins(limo_data *env)
{
  int i;

  mp_set_memory_functions (GC_malloc, 
			   limo_gmp_gc_realloc, 
			   limo_gmp_gc_free);

  for (i=0; 
       i<(sizeof number_builtin_array)/(sizeof number_builtin_array[0]); 
       ++i)
    setq(env, make_sym(number_builtin_array[i].name), 
	 make_builtin(number_builtin_array[i].f));
}

