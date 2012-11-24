#include "limo.h"

struct { char *name; limo_builtin f; } builtin_array[] = {
  { "QUOTE", builtin_quote },
  { "SETQ", builtin_setq },
  { "SETF", builtin_setf },
  { "SETCAR", builtin_setcar },
  { "SETCDR", builtin_setcdr },
  { "IF", builtin_if },
  { "LAMBDA", builtin_lambda },
  { "MACRO", builtin_macro },
  { "MACROEXPAND-1", builtin_macroexpand_1 },
  { "EVAL", builtin_eval},
  { "APPLY", builtin_apply },
  { "PROGN", builtin_progn},
  { "LIST", builtin_list },
  { "CONS", builtin_cons}, 
  { "DCONS", builtin_dcons },
  { "CONSP", builtin_consp}, 
  { "CAR", builtin_car},
  { "CDR", builtin_cdr},
  { "EQ", builtin_eq},
  { "WRITE", builtin_write},
  { "TRY", builtin_try },
  { "THROW", builtin_throw },
  { "EXIT", builtin_exit },
  { "LOAD", builtin_load},
  { "LOADDLL", builtin_loaddll },
  { "GCENABLE", builtin_gc_enable },
  { "GCDISABLE", builtin_gc_disable },
  { "EXTRACT-ENV", builtin_extract_env },
};

limo_data *make_globalenv(int argc, char **argv)
{
  limo_data *env = make_env(NULL);
  limo_data *args_start;
  limo_data **args = &args_start;
  int i;

  for (i=0; i<(sizeof builtin_array)/(sizeof builtin_array[0]); ++i)
    setq(env, make_sym(builtin_array[i].name), make_builtin(builtin_array[i].f));

  for (i=1; i<argc; ++i) {
    (*args) = make_cons(make_string(argv[i]), NULL);
    args = &CDR(*args);
  }
  (*args) = make_nil();

  setq(env, make_sym("ARGV"), args_start);
  setq(env, make_sym("_TRACE"), make_nil());
  setq(env, make_sym("_INTERNED-SYMBOLS"), interned_symbols);

  number_builtins(env);

  return env;
}
