#include <limo.h>
#include <mhash.h>

#define INSBUILTIN(f, name) setq(env, make_sym(name), make_builtin(f, name))
#define INSBUILTINFUN(f, name) setq(env, make_sym(name), make_builtinfun(f, name))

limo_data *sym_mhash;

static void *hash_malloc(mutils_word32 size)
{
  return GC_malloc(size);
}

BUILTINFUN(builtin_mhash_init)
{
  int hashtype;
  MHASH mh;

  REQUIRE_ARGC_FUN("MHASH-INIT", 1);
  REQUIRE_NUMBER("MHASH-INIT", argv[0]);

  hashtype = GETINTFROMNUMBER(argv[0]);

  mh = mhash_init(hashtype);

  return make_special(sym_mhash, mh);
}

BUILTINFUN(builtin_mhash)
{
  int i;
  REQUIRE_ARGC_FUN("MHASH", 2);
  REQUIRE_TYPE("MHASH", argv[0], limo_TYPE_SPECIAL);
  REQUIRE_TYPE("MHASH", argv[1], limo_TYPE_STRING);

  i = mhash(get_special(argv[0], sym_mhash), argv[1]->d_string, argv[1]->string_length);

  return make_rational_from_long_long(i);
}

BUILTINFUN(builtin_mhash_end)
{
  char *digest;
  limo_data *res;
  MHASH mh;

  REQUIRE_ARGC_FUN("MHASH-END", 1);
  REQUIRE_TYPE("MHASH-END", argv[0], limo_TYPE_SPECIAL);

  mh = get_special(argv[0], sym_mhash);
  digest = mhash_end_m(mh, hash_malloc);

  res = make_limo_data();
  res->type = limo_TYPE_STRING;
  res->string_length = mhash_get_block_size(mhash_get_mhash_algo(mh));
  res->d_string = digest;

  return res;
}

void limo_init_mhash(limo_data *env)
{
  limo_data *ld_mhash_env;

  sym_mhash = make_sym("MHASH");
  ld_mhash_env = make_env(nil);

  INSBUILTINFUN(builtin_mhash_init, "MHASH-INIT");
  INSBUILTINFUN(builtin_mhash_end, "MHASH-END");
  INSBUILTINFUN(builtin_mhash, "MHASH");

  setq(ld_mhash_env, make_sym("MHASH_CRC32"), make_rational_from_long_long(MHASH_CRC32));
  setq(ld_mhash_env, make_sym("MHASH_CRC32B"), make_rational_from_long_long(MHASH_CRC32B));
  setq(ld_mhash_env, make_sym("MHASH_ADLER32"), make_rational_from_long_long(MHASH_ADLER32));
  setq(ld_mhash_env, make_sym("MHASH_MD2"), make_rational_from_long_long(MHASH_MD2));
  setq(ld_mhash_env, make_sym("MHASH_MD4"), make_rational_from_long_long(MHASH_MD4));
  setq(ld_mhash_env, make_sym("MHASH_RIPEMD128"), make_rational_from_long_long(MHASH_RIPEMD128));
  setq(ld_mhash_env, make_sym("MHASH_RIPEMD160"), make_rational_from_long_long(MHASH_RIPEMD160));
  setq(ld_mhash_env, make_sym("MHASH_RIPEMD256"), make_rational_from_long_long(MHASH_RIPEMD256));
  setq(ld_mhash_env, make_sym("MHASH_RIPEMD320"), make_rational_from_long_long(MHASH_RIPEMD320));
  setq(ld_mhash_env, make_sym("MHASH_MD5"), make_rational_from_long_long(MHASH_MD5));
  setq(ld_mhash_env, make_sym("MHASH_SHA1"), make_rational_from_long_long(MHASH_SHA1));
  setq(ld_mhash_env, make_sym("MHASH_SHA224"), make_rational_from_long_long(MHASH_SHA224));
  setq(ld_mhash_env, make_sym("MHASH_SHA256"), make_rational_from_long_long(MHASH_SHA256));
  setq(ld_mhash_env, make_sym("MHASH_SHA384"), make_rational_from_long_long(MHASH_SHA384));
  setq(ld_mhash_env, make_sym("MHASH_SHA512"), make_rational_from_long_long(MHASH_SHA512));
  setq(ld_mhash_env, make_sym("MHASH_HAVAL128"), make_rational_from_long_long(MHASH_HAVAL128));
  setq(ld_mhash_env, make_sym("MHASH_HAVAL160"), make_rational_from_long_long(MHASH_HAVAL160));
  setq(ld_mhash_env, make_sym("MHASH_HAVAL192"), make_rational_from_long_long(MHASH_HAVAL192));
  setq(ld_mhash_env, make_sym("MHASH_HAVAL224"), make_rational_from_long_long(MHASH_HAVAL224));
  setq(ld_mhash_env, make_sym("MHASH_HAVAL256"), make_rational_from_long_long(MHASH_HAVAL256));
  setq(ld_mhash_env, make_sym("MHASH_TIGER128"), make_rational_from_long_long(MHASH_TIGER128));
  setq(ld_mhash_env, make_sym("MHASH_TIGER160"), make_rational_from_long_long(MHASH_TIGER160));
  setq(ld_mhash_env, make_sym("MHASH_TIGER192"), make_rational_from_long_long(MHASH_TIGER192));
  setq(ld_mhash_env, make_sym("MHASH_GOST"), make_rational_from_long_long(MHASH_GOST));
  setq(ld_mhash_env, make_sym("MHASH_WHIRLPOOL"), make_rational_from_long_long(MHASH_WHIRLPOOL));
  setq(ld_mhash_env, make_sym("MHASH_SNEFRU128"), make_rational_from_long_long(MHASH_SNEFRU128));
  setq(ld_mhash_env, make_sym("MHASH_SNEFRU256"), make_rational_from_long_long(MHASH_SNEFRU256));

  setq(env, make_sym("_MHASH"), ld_mhash_env);
}
