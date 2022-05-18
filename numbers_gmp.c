#include <math.h>
#include "limo.h"

BUILTINFUN(builtin_numberp)
{
  REQUIRE_ARGC_FUN("NUMBERP", 1);
  if (argv[0]->type == limo_TYPE_GMPQ)
    return sym_true;
  else
    return nil;
}

BUILTINFUN(builtin_reprn)
{
  REQUIRE_ARGC_FUN("REPRN", 1);
  return make_string(repr_number(argv[0]));
}

BUILTINFUN(builtin_idivmod)
{
  limo_data *q = make_number();
  limo_data *r = make_number();
  limo_data *res = make_cons(q, make_cons(r, nil));

  REQUIRE_ARGC_FUN("IDIVMOD", 2);
  limo_data *n = argv[0];
  limo_data *d = argv[1];
  REQUIRE_TYPE("IDIVMOD", n, limo_TYPE_GMPQ);
  REQUIRE_TYPE("IDIVMOD", d, limo_TYPE_GMPQ);

  if (!mpz_cmp_si(mpq_numref(LIMO_MPQ(d)), 0))
    limo_error("integer division by zero");

  mpz_tdiv_qr(mpq_numref(LIMO_MPQ(q)),
	      mpq_numref(LIMO_MPQ(r)),
	      mpq_numref(LIMO_MPQ(n)),
	      mpq_numref(LIMO_MPQ(d)));
  return res;
}

BUILTINFUN(builtin_mpq_numerator) {
  limo_data *arg, *res;

  REQUIRE_ARGC_FUN("MPQ_NUMERATOR", 1);
  arg = argv[0];
  REQUIRE_TYPE("MPQ_NUMERATOR", arg, limo_TYPE_GMPQ);
  res = make_number();  // 0/1
  mpq_set_num(LIMO_MPQ(res), mpq_numref(LIMO_MPQ(arg)));
  return res;
}

BUILTINFUN(builtin_mpq_denominator)
{
  limo_data *arg, *res;

  REQUIRE_ARGC_FUN("MPQ_DENONINATOR", 1);
  arg = argv[0];
  REQUIRE_TYPE("MPQ_DENONINATOR", arg, limo_TYPE_GMPQ);
  res = make_number();  // 0/1
  mpq_set_num(LIMO_MPQ(res), mpq_denref(LIMO_MPQ(arg)));
  return res;
}

BUILTINFUN(builtin_mpq_add)
{
  int i;
  limo_data *res = make_number();
  for (i=0; i<argc; ++i) {
    REQUIRE_TYPE("MPQ_ADD", argv[i], limo_TYPE_GMPQ);
    mpq_add(LIMO_MPQ(res), LIMO_MPQ(res), LIMO_MPQ(argv[i]));
  }
  return res;

}

BUILTINFUN(builtin_mpq_mul)
{
  int i;
  limo_data *res = make_number();
  mpq_set_ui(LIMO_MPQ(res), 1, 1);
  for (i=0; i<argc; ++i) {
    REQUIRE_TYPE("MPQ_MUL", argv[i], limo_TYPE_GMPQ);
    mpq_mul(LIMO_MPQ(res), LIMO_MPQ(res), LIMO_MPQ(argv[i]));
  }

  return res;
}

BUILTINFUN(builtin_mpq_sub)
{
  int i;
  limo_data *res = make_number();
  switch (argc) {
  case 0:
    limo_error("MPQ_SUB needs at least one argument");
    break;

  case 1:
    REQUIRE_TYPE("MPQ_SUB", argv[0], limo_TYPE_GMPQ);    
    mpq_neg(LIMO_MPQ(res), LIMO_MPQ(argv[0]));
    return res;

  default:
    REQUIRE_TYPE("MPQ_SUB", argv[0], limo_TYPE_GMPQ);
    mpq_set(LIMO_MPQ(res), LIMO_MPQ(argv[0]));
    for (i=1; i<argc; ++i) {
      REQUIRE_TYPE("MPQ_SUB", argv[i], limo_TYPE_GMPQ);
      mpq_sub(LIMO_MPQ(res), LIMO_MPQ(res), LIMO_MPQ(argv[i]));
    }
    return res;
  }
}

BUILTINFUN(builtin_mpq_div)
{
  int i;
  limo_data *res = make_number();
  switch (argc) {
  case 0:
    limo_error("MPQ_DIV needs at least one argument");
    break;

  case 1:
    REQUIRE_TYPE("MPQ_DIV", argv[0], limo_TYPE_GMPQ);
    if (mpq_equal(LIMO_MPQ(res), LIMO_MPQ(argv[0])))  // zero?
      limo_error("MPQ_DIV division by zero");
    
    mpq_inv(LIMO_MPQ(res), LIMO_MPQ(argv[0]));
    return res;

  default:
    REQUIRE_TYPE("MPQ_DIV", argv[0], limo_TYPE_GMPQ);    
    mpq_set(LIMO_MPQ(res), LIMO_MPQ(argv[0]));
    for (i=1; i<argc; ++i) {
      REQUIRE_TYPE("MPQ_DIV", argv[i], limo_TYPE_GMPQ);
      if (mpq_sgn(LIMO_MPQ(argv[i])) == 0)
        limo_error("MPQ_DIV division by zero");
      mpq_div(LIMO_MPQ(res), LIMO_MPQ(res), LIMO_MPQ(argv[i]));
    }
    return res;
  }
}

#define CALC1_BUILTINFUN(fun) BUILTINFUN(builtin_##fun ) \
{ \
  limo_data *res = make_number(); \
  limo_data *arg;		  \
  if (argc != 1) \
    limo_error(#fun " needs 1 arg!"); \
  arg = argv[0]; \
  REQUIRE_TYPE(#fun, arg, limo_TYPE_GMPQ); \
  fun(LIMO_MPQ(res), LIMO_MPQ(arg)); \
  return res; \
}

CALC1_BUILTINFUN(mpq_neg)
CALC1_BUILTINFUN(mpq_abs)
CALC1_BUILTINFUN(mpq_inv)

BUILTINFUN(builtin_ltn)
{
  REQUIRE_ARGC_FUN("LTN", 2);
  if (mpq_cmp(LIMO_MPQ(argv[0]), LIMO_MPQ(argv[1])) < 0)
    return sym_true;
  else
    return nil;
}

BUILTINFUN(builtin_gtn)
{
  REQUIRE_ARGC_FUN("GTN", 2);
  if (mpq_cmp(LIMO_MPQ(argv[0]), LIMO_MPQ(argv[1])) > 0)
    return sym_true;
  else
    return nil;
}

BUILTINFUN(builtin_sin)
{
  REQUIRE_ARGC_FUN("SIN", 1);
  return make_number_from_double(sin(make_double_from_number(argv[0])));
}

BUILTINFUN(builtin_cos)
{
  REQUIRE_ARGC_FUN("COS", 1);
  return make_number_from_double(cos(make_double_from_number(argv[0])));
}

BUILTINFUN(builtin_atan)
{
  REQUIRE_ARGC_FUN("ATAN", 1);
  return make_number_from_double(atan(make_double_from_number(argv[0])));
}

BUILTINFUN(builtin_asin)
{
  REQUIRE_ARGC_FUN("ASIN", 1);
  return make_number_from_double(asin(make_double_from_number(argv[0])));
}

BUILTINFUN(builtin_power)
{
  REQUIRE_ARGC_FUN("POWER", 2);
  return make_number_from_double(pow(make_double_from_number(argv[0]),
				     make_double_from_number(argv[1])));
}

BUILTINFUN(builtin_int)
{
  REQUIRE_ARGC_FUN("INT", 1);
  return make_number_from_double((double)(long long)(make_double_from_number(argv[0])));
}


////////////////////
// Testfunctions
BUILTINFUN(builtin_test_todbl) {
  limo_data *res = make_limo_data();
  REQUIRE_ARGC_FUN("TEST-TODBL", 1);
  res->type = limo_TYPE_DOUBLE;
  res->d_double = mpq_get_d(LIMO_MPQ(argv[0]));
  return res;
}

BUILTINFUN(builtin_test_toint) {
  limo_data *res = make_limo_data();
  REQUIRE_ARGC_FUN("TEST-TOINT", 1);
  res->type = limo_TYPE_INT;
  res->d_int = mpz_get_si(mpq_numref(LIMO_MPQ(argv[0])));
  return res;
}

BUILTINFUN(builtin_test_intltn)
{
  REQUIRE_ARGC_FUN("TEST-INTLTN", 2);
  if (argv[0]->d_int < argv[1]->d_int)
    return sym_true;
  else
    return nil;
}

BUILTINFUN(builtin_test_intadd)
{
  REQUIRE_ARGC_FUN("TEST-INTADD", 2);
  limo_data *res = make_limo_data();
  res->type = limo_TYPE_INT;
  res->d_int = argv[0]->d_int + argv[1]->d_int;
  return res;
}

////////////////////

limo_data *make_number(void)
{
  limo_data *res = make_limo_data();
  void **make_gmpq_next = pk_gmpq_next_get();

  if (!*make_gmpq_next)
    *make_gmpq_next = GC_malloc_many(sizeof (mpq_t));

  res->type=limo_TYPE_GMPQ;
  res->d_mpq = (mpq_t *)*make_gmpq_next;
  *make_gmpq_next = GC_NEXT(*make_gmpq_next);

  mpq_init(LIMO_MPQ(res)); // initializes to 0/1
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

double make_double_from_number(limo_data *n)
{
  double d;
  d = mpq_get_d(LIMO_MPQ(n));

  return d;
}

struct { char *name; limo_builtinfun f; } number_builtin_array[] = {
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
  { "MPQ_NUMERATOR", builtin_mpq_numerator },
  { "MPQ_DENOMINATOR", builtin_mpq_denominator },
  { "SIN", builtin_sin },
  { "COS", builtin_cos },
  { "ATAN", builtin_atan },
  { "ASIN", builtin_asin },
  { "POWER", builtin_power },
  { "INT", builtin_int },

  /// new double and int testfuns
  { "TEST-TOINT", builtin_test_toint },
  { "TEST-TODBL", builtin_test_todbl },
  { "TEST-INTLTN", builtin_test_intltn },
  { "TEST-INTADD", builtin_test_intadd },
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
	 make_builtinfun(number_builtin_array[i].f));
}
