#include <limo.h>

#define INS_SPEEDUP_BUILTIN(f, name) setq(limo_speedup_env, make_sym(name), make_builtin(f, name))
#define INS_SPEEDUP_BUILTINFUN(f, name) setq(limo_speedup_env, make_sym(name), make_builtinfun(f, name))
#define INS_SPEEDUP_VAR(val, name)   setq(limo_speedup_env, make_sym(name), val)

limo_data *sym_speedup;

static limo_data *drange;
BUILTINFUN(builtin_drange)
{
  REQUIRE_ARGC_FUN("DRANGE", 3);
  REQUIRE_TYPE("DRANGE", argv[0], limo_TYPE_GMPQ);
  REQUIRE_TYPE("DRANGE", argv[1], limo_TYPE_GMPQ);
  REQUIRE_TYPE("DRANGE", argv[2], limo_TYPE_GMPQ);
  if (mpq_cmp(LIMO_MPQ(argv[0]), LIMO_MPQ(argv[1])) >= 0)
    return nil;
  else {
    limo_data *r = make_rational();
    mpq_add(LIMO_MPQ(r), LIMO_MPQ(argv[0]), LIMO_MPQ(argv[2]));
    return make_dcons(argv[0], make_list(0, drange, r, argv[1], argv[2], NULL), nil);
  }
}

BUILTINFUN(builtin_undyn)
{
  limo_data *ld, *list=argv[0];
  limo_data **el = &ld;
  
  REQUIRE_ARGC_FUN("UNDYN", 1);

  while (list->type == limo_TYPE_CONS) {
    (*el) = make_cons(list->car, NULL);

    list = list->cdr;
    if (list->type == limo_TYPE_THUNK)
      list = eval(list->cdr, list->car);

    el = &CDR(*el);
  }
  (*el) = list;

  return ld;
}

void limo_init_speedup(limo_data *env)
{
  limo_data *limo_speedup_env;
  sym_speedup = make_sym("SPEEDUP");

  drange = make_builtinfun(builtin_drange, "DRANGE");

  limo_speedup_env = make_env(nil);
  
  INS_SPEEDUP_BUILTINFUN(builtin_drange, "DRANGE");
  INS_SPEEDUP_BUILTINFUN(builtin_undyn, "UNDYN");

  setq(env, make_sym("_SPEEDUP"), limo_speedup_env);
}
