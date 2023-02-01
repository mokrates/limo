#include <math.h>
#include "limo.h"

#define ORDER_RATIONAL_RATIONAL ((limo_TYPE_GMPQ << 8)   | limo_TYPE_GMPQ)
#define ORDER_RATIONAL_FLOAT    ((limo_TYPE_GMPQ << 8)   | limo_TYPE_DOUBLE)
#define ORDER_FLOAT_RATIONAL    ((limo_TYPE_DOUBLE << 8) | limo_TYPE_GMPQ)
#define ORDER_FLOAT_FLOAT       ((limo_TYPE_DOUBLE << 8) | limo_TYPE_DOUBLE)

BUILTINFUN(builtin_numberp)
{
  REQUIRE_ARGC_FUN("NUMBERP", 1);
  if (argv[0]->type == limo_TYPE_GMPQ || argv[0]->type == limo_TYPE_DOUBLE)
    return sym_true;
  else
    return nil;
}

BUILTINFUN(builtin_floatp)
{
  REQUIRE_ARGC_FUN("FLOATP", 1);
  if (argv[0]->type == limo_TYPE_DOUBLE)
    return sym_true;
  else
    return nil;
}

BUILTINFUN(builtin_rationalp)
{
  REQUIRE_ARGC_FUN("RATIONALP", 1);
  if (argv[0]->type == limo_TYPE_GMPQ)
    return sym_true;
  else
    return nil;
}

BUILTINFUN(builtin_int)
{
  REQUIRE_ARGC_FUN("INT", 1);
  return make_rational_from_long_long((long long)(COERCETODOUBLE(argv[0])));
}

BUILTINFUN(builtin_float)
{
  REQUIRE_ARGC_FUN("FLOAT", 1);
  REQUIRE_NUMBER("FLOAT", argv[0]);
  return argv[0]->type == limo_TYPE_DOUBLE ? argv[0] : make_float(COERCETODOUBLE(argv[0]));
}

BUILTINFUN(builtin_rational)
{
  REQUIRE_ARGC_FUN("RATIONAL", 1);
  REQUIRE_NUMBER("RATIONAL", argv[0]);
  return argv[0]->type == limo_TYPE_GMPQ ? argv[0] : make_rational_from_double(argv[0]->d_double);
}

BUILTINFUN(builtin_reprn)
{
  REQUIRE_ARGC_FUN("REPRN", 1);
  return make_string(repr_number(argv[0]));
}

BUILTINFUN(builtin_idivmod)
{
  limo_data *q = make_rational();
  limo_data *r = make_rational();
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
  res = make_rational();  // 0/1
  mpq_set_num(LIMO_MPQ(res), mpq_numref(LIMO_MPQ(arg)));
  return res;
}

BUILTINFUN(builtin_mpq_denominator)
{
  limo_data *arg, *res;

  REQUIRE_ARGC_FUN("MPQ_DENONINATOR", 1);
  arg = argv[0];
  REQUIRE_TYPE("MPQ_DENONINATOR", arg, limo_TYPE_GMPQ);
  res = make_rational();  // 0/1
  mpq_set_num(LIMO_MPQ(res), mpq_denref(LIMO_MPQ(arg)));
  return res;
}

BUILTINFUN(builtin_add)
{
  limo_data *res;
  
  res = make_rational();

  while (argc-- > 0) {
    REQUIRE_NUMBER("_ADD", argv[0]);    
    if (((res->type << 8) | argv[0]->type) == ORDER_RATIONAL_RATIONAL)
      mpq_add(LIMO_MPQ(res), LIMO_MPQ(res), LIMO_MPQ(argv[0]));
    else
      res = make_float(COERCETODOUBLE(res) + COERCETODOUBLE(argv[0]));

    ++argv;
  }
  return res;
}

BUILTINFUN(builtin_mul)
{
  limo_data *res;
  
  res = make_rational();
  mpq_set_ui(LIMO_MPQ(res), 1, 1);

  while (argc-- > 0) {
    REQUIRE_NUMBER("_MUL", argv[0]);
    if (((res->type << 8) | argv[0]->type) == ORDER_RATIONAL_RATIONAL)
      mpq_mul(LIMO_MPQ(res), LIMO_MPQ(res), LIMO_MPQ(argv[0]));
    else
      res = make_float(COERCETODOUBLE(res) * COERCETODOUBLE(argv[0]));

    ++argv;
  }
  return res;
}

BUILTINFUN(builtin_sub)
{
  limo_data *res;
  if (argc == 1) {
    switch (argv[0]->type) {
    case limo_TYPE_GMPQ: {
      res = make_rational();
      mpq_inv(LIMO_MPQ(res), LIMO_MPQ(argv[0]));
      return res;
    }

    case limo_TYPE_DOUBLE:
      return make_float(-(argv[0]->d_double));

    default:
      limo_error("_SUB expects NUMBERS");
    }
  }
  else {
    REQUIRE_NUMBER("_SUB", argv[0]);
    REQUIRE_NUMBER("_SUB", argv[1]);
    
    if (((argv[0]->type << 8) | argv[1]->type) == ORDER_RATIONAL_RATIONAL) {
      res = make_rational();
      mpq_sub(LIMO_MPQ(res), LIMO_MPQ(argv[0]), LIMO_MPQ(argv[1]));
    }
    else
      res = make_float(COERCETODOUBLE(argv[0]) - COERCETODOUBLE(argv[1]));
    return res;
  }
}

BUILTINFUN(builtin_div)
{
  limo_data *res;
  REQUIRE_ARGC_FUN("_DIV", 2);
  REQUIRE_NUMBER("_DIV", argv[0]);
  REQUIRE_NUMBER("_DIV", argv[1]);

  if (((argv[0]->type << 8) | argv[1]->type) == ORDER_RATIONAL_RATIONAL) {
    res = make_rational();
    if (mpq_sgn(LIMO_MPQ(argv[1])) == 0)
      limo_error("MPQ_DIV division by zero");
    mpq_div(LIMO_MPQ(res), LIMO_MPQ(argv[0]), LIMO_MPQ(argv[1]));
  }
  else {
    double denom = COERCETODOUBLE(argv[1]);
    if (denom == 0)
      limo_error("MPQ_DIV division by zero");
    res = make_float(COERCETODOUBLE(argv[0]) / denom);
  }
  return res;
}

BUILTINFUN(builtin_ltn)
{
  REQUIRE_ARGC_FUN("LTN", 2);
  REQUIRE_NUMBER("LTN", argv[0]);
  REQUIRE_NUMBER("LTN", argv[1]);

  if (((argv[0]->type << 8) | argv[1]->type) == ORDER_RATIONAL_RATIONAL) {
    if (mpq_cmp(LIMO_MPQ(argv[0]), LIMO_MPQ(argv[1])) < 0)
      return sym_true;
    else
      return nil;
  }
  else {
    if (COERCETODOUBLE(argv[0]) < COERCETODOUBLE(argv[1]))
      return sym_true;
    else
      return nil;
  }
}

BUILTINFUN(builtin_gtn)
{
  REQUIRE_ARGC_FUN("GTN", 2);
  REQUIRE_NUMBER("GTN", argv[0]);
  REQUIRE_NUMBER("GTN", argv[1]);

  if (((argv[0]->type << 8) | argv[1]->type) == ORDER_RATIONAL_RATIONAL) {
    if (mpq_cmp(LIMO_MPQ(argv[0]), LIMO_MPQ(argv[1])) > 0)
      return sym_true;
    else
      return nil;
  }
  else {
    if (COERCETODOUBLE(argv[0]) > COERCETODOUBLE(argv[1]))
      return sym_true;
    else
      return nil;
  }
}

BUILTINFUN(builtin_sin)
{
  REQUIRE_ARGC_FUN("SIN", 1);
  return make_float(sin(COERCETODOUBLE(argv[0])));
}

BUILTINFUN(builtin_cos)
{
  REQUIRE_ARGC_FUN("COS", 1);
  return make_float(cos(COERCETODOUBLE(argv[0])));
}

BUILTINFUN(builtin_atan)
{
  REQUIRE_ARGC_FUN("ATAN", 1);
  return make_float(atan(COERCETODOUBLE(argv[0])));
}

BUILTINFUN(builtin_asin)
{
  REQUIRE_ARGC_FUN("ASIN", 1);
  return make_float(asin(COERCETODOUBLE(argv[0])));
}

BUILTINFUN(builtin_power)
{
  REQUIRE_ARGC_FUN("POWER", 2);
  return make_float(pow(COERCETODOUBLE(argv[0]),
                        COERCETODOUBLE(argv[1])));
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

limo_data *make_rational(void)
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

limo_data *make_float(double d)
{
  limo_data *res = make_limo_data();
  res->type=limo_TYPE_DOUBLE;
  res->d_double = d;
  return res;
}
 
char *repr_number(limo_data *ld)
{
  if (ld->type == limo_TYPE_GMPQ)
    return mpq_get_str(NULL, 10, LIMO_MPQ(ld));
  else {
    limo_data *format, *dyn_env = pk_dynamic_vars_get();
    char *output;
    int length;
    
    format=var_lookup(dyn_env, make_sym("_FLOAT-WRITER-FORMAT"));
    if (!format)
      limo_error("_FLOAT-WRITER-FORMAT not defined in _DYN-ENV");
    if (format->type != limo_TYPE_STRING)
      limo_error("_FLOAT-WRITER-FORMAT not a string");  
    length = snprintf(NULL, 0, format->d_string, ld->d_double);
    output = GC_malloc(length+1); // '\0'-terminator
    snprintf(output, length+1, format->d_string, ld->d_double);
    return output;
  }
}

limo_data *make_rational_from_str(char *str)
{
  limo_data *res = make_rational();

  mpq_set_str(LIMO_MPQ(res), str, 10);
  mpq_canonicalize(LIMO_MPQ(res));
  return res;
}

limo_data *make_rational_from_long_long(long long i)
{
  limo_data *res = make_rational();

  mpq_set_d(LIMO_MPQ(res), (double)i);
  mpq_canonicalize(LIMO_MPQ(res));
  return res;
}

limo_data *make_rational_from_double(double d)
{
  limo_data *res = make_rational();

  mpq_set_d(LIMO_MPQ(res), (double)d);
  mpq_canonicalize(LIMO_MPQ(res));
  return res;
}

double make_double_from_rational(limo_data *n)
{
  double d;
  d = mpq_get_d(LIMO_MPQ(n));

  return d;
}

struct { char *name; limo_builtinfun f; } number_builtin_array[] = {
  { "NUMBERP", builtin_numberp },
  { "FLOATP", builtin_floatp },
  { "RATIONALP", builtin_rationalp },

  { "FLOAT", builtin_float },
  { "RATIONAL", builtin_rational },
  { "INT", builtin_int },
  
  { "REPRN", builtin_reprn },
  { "LTN", builtin_ltn },
  { "GTN", builtin_gtn },
  { "_ADD", builtin_add },
  { "_SUB", builtin_sub },
  { "_MUL", builtin_mul },
  { "_DIV", builtin_div },
  { "IDIVMOD", builtin_idivmod },
  { "MPQ_NUMERATOR", builtin_mpq_numerator },
  { "MPQ_DENOMINATOR", builtin_mpq_denominator },
  { "SIN", builtin_sin },
  { "COS", builtin_cos },
  { "ATAN", builtin_atan },
  { "ASIN", builtin_asin },
  { "POWER", builtin_power },


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
