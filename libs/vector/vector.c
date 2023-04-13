#include <limo.h>

#define INSBUILTIN(f, name) setq(env, make_sym(name), make_builtin(f, name))
#define INSBUILTINFUN(f, name) setq(env, make_sym(name), make_builtinfun(f, name))

limo_data *sym_vector;

struct VECTOR {
  unsigned long length;
  limo_data **contents;
};

BUILTIN(builtin_make_vector)
{
  limo_data *len;
  unsigned long i;
  limo_data *result;
  struct VECTOR *v;

  REQUIRE_ARGC("MAKE-VECTOR", 1);
  len = eval(FIRST_ARG, env);
  if (len->type != limo_TYPE_GMPQ)
    limo_error("(make-vector LENGTH)");

  i = (unsigned long)mpq_get_d(*len->d_mpq);

  v = GC_malloc(sizeof (struct VECTOR));
  v->length = i;
  v->contents = (limo_data **)memset(GC_malloc(i * sizeof (limo_data *)), 0, i*sizeof (limo_data *));
  return make_special(sym_vector, v);
}

BUILTIN(builtin_vector_len)
{
  limo_data *vector;

  REQUIRE_ARGC("VECTOR-LEN", 1);
  vector = eval(FIRST_ARG, env);
  return make_rational_from_long_long(((struct VECTOR *)get_special(vector, sym_vector))->length);
}

BUILTIN(builtin_vector_set)
{
  limo_data *ld_vec, *ld_n, *ld_val;
  unsigned long n;
  struct VECTOR *vec;

  REQUIRE_ARGC("VECTOR-SET", 3);
  ld_vec = eval(FIRST_ARG, env);
  ld_n   = eval(SECOND_ARG, env);
  ld_val = eval(THIRD_ARG, env);

  n = GETINTFROMNUMBER(ld_n);
  vec = get_special(ld_vec, sym_vector);
  if (!(n<vec->length))
    limo_error("VECTOR-SET: Index out of bounds!");
  vec->contents[n] = ld_val;

  return ld_val;
}

BUILTIN(builtin_vector_ref)
{
  limo_data *ld_vec, *ld_n, *ld_val;
  unsigned long n;
  struct VECTOR *vec;

  REQUIRE_ARGC("VECTOR-REF", 2);
  ld_vec = eval(FIRST_ARG, env);
  ld_n   = eval(SECOND_ARG, env);

  n = GETINTFROMNUMBER(ld_n);
  vec = get_special(ld_vec, sym_vector);
  if (!(n<vec->length))
    limo_error("VECTOR-REF: Index out of bounds!");
  return vec->contents[n]?vec->contents[n]:nil;
}

BUILTINFUN(builtin_vectorp)
{
  REQUIRE_ARGC_FUN("VECTORP", 1);
  if (argv[0]->type != limo_TYPE_SPECIAL)
    return nil;
  else
    if (limo_equals(CAR(argv[0]), sym_vector))
      return sym_true;
    else
      return nil;
}

void limo_init_vector(limo_data *env)
{
  sym_vector = make_sym("VECTOR");
  INSBUILTIN(builtin_make_vector, "MAKE-VECTOR");
  INSBUILTIN(builtin_vector_ref, "VECTOR-REF");
  INSBUILTIN(builtin_vector_set, "VECTOR-SET");
  INSBUILTIN(builtin_vector_len, "VECTOR-LEN");
  INSBUILTINFUN(builtin_vectorp, "VECTORP");
}
