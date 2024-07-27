#include <limo.h>

#define INS_BO_BUILTINFUN(f, name) setq(limo_bo_env, make_sym(name), make_builtinfun(f, name))
#define INS_BO_VAR(val, name)   setq(limo_bo_env, make_sym(name), val)

limo_data *sym_bo;

// mask. use
// #xFF for 8 bits
// #xFFFF for 16 bits
// #xFFFFFFFF for 32 bits
// #xFFFFFFFFFFFFFFFF for 64 bits
// define this in bitops.limo

BUILTINFUN(builtin_bitops_and)
{
  REQUIRE_ARGC_FUN("BITOPS-AND", 3);
  REQUIRE_TYPE("BITOPS-AND", argv[0], limo_TYPE_GMPQ);
  REQUIRE_TYPE("BITOPS-AND", argv[1], limo_TYPE_GMPQ);
  REQUIRE_TYPE("BITOPS-AND", argv[2], limo_TYPE_GMPQ);

  unsigned long long x, y, mask;
  mask = GETINTFROMMPQ(argv[0]);
  x = GETINTFROMMPQ(argv[1]);
  y = GETINTFROMMPQ(argv[2]);

  return make_rational_from_long_long(x & y & mask);
}

BUILTINFUN(builtin_bitops_or)
{
  REQUIRE_ARGC_FUN("BITOPS-OR", 3);
  REQUIRE_TYPE("BITOPS-OR", argv[0], limo_TYPE_GMPQ);
  REQUIRE_TYPE("BITOPS-OR", argv[1], limo_TYPE_GMPQ);
  REQUIRE_TYPE("BITOPS-OR", argv[2], limo_TYPE_GMPQ);

  unsigned long long x, y, mask;
  mask = GETINTFROMMPQ(argv[0]);
  x = GETINTFROMMPQ(argv[1]);
  y = GETINTFROMMPQ(argv[2]);

  return make_rational_from_long_long((x | y) & mask);
}

BUILTINFUN(builtin_bitops_xor)
{
  REQUIRE_ARGC_FUN("BITOPS-XOR", 3);
  REQUIRE_TYPE("BITOPS-XOR", argv[0], limo_TYPE_GMPQ);
  REQUIRE_TYPE("BITOPS-XOR", argv[1], limo_TYPE_GMPQ);
  REQUIRE_TYPE("BITOPS-XOR", argv[2], limo_TYPE_GMPQ);

  unsigned long long x, y, mask;
  mask = GETINTFROMMPQ(argv[0]);
  x = GETINTFROMMPQ(argv[1]);
  y = GETINTFROMMPQ(argv[2]);

  return make_rational_from_long_long((x ^ y) & mask);
}

BUILTINFUN(builtin_bitops_not)
{
  REQUIRE_ARGC_FUN("BITOPS-NOT", 2);
  REQUIRE_TYPE("BITOPS-NOT", argv[0], limo_TYPE_GMPQ);
  REQUIRE_TYPE("BITOPS-NOT", argv[1], limo_TYPE_GMPQ);

  unsigned long long x, y, mask;
  mask = GETINTFROMMPQ(argv[0]);
  x = GETINTFROMMPQ(argv[1]);

  return make_rational_from_long_long((~x) & mask);
}

void limo_init_bitops(limo_data *env)
{
  limo_data *limo_bo_env;
  sym_bo = make_sym("BITOPS");
  limo_bo_env = make_env(nil);
 
  INS_BO_BUILTINFUN(builtin_bitops_and, "BITOPS-AND");
  INS_BO_BUILTINFUN(builtin_bitops_or, "BITOPS-OR");
  INS_BO_BUILTINFUN(builtin_bitops_xor, "BITOPS-XOR");
  INS_BO_BUILTINFUN(builtin_bitops_not, "BITOPS-NOT");

  setq(env, make_sym("_BITOPS"), limo_bo_env);
}
