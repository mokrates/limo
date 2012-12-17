#include "limo.h"

struct { char *name; limo_builtin f; } builtin_array[] = {
  { "QUOTE", builtin_quote },
  { "SETQ", builtin_setq },
  { "UNSETQ", builtin_unsetq },
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
  { "FINALLY", builtin_finally},
  { "THROW", builtin_throw },
  { "EXIT", builtin_exit },
  { "LOAD", builtin_load},
  { "LOADDLL", builtin_loaddll },
  { "GCENABLE", builtin_gc_enable },
  { "GCDISABLE", builtin_gc_disable },
  { "GCCOLLECT", builtin_gc_collect},
  { "GCSETMAX", builtin_gc_setmax},
  { "EXTRACT-ENV", builtin_extract_env },
  { "SLEEP", builtin_sleep },
  { "STRING-CONCAT", builtin_string_concat },
  { "MAKE-SYM", builtin_make_sym },
  { "GET-ANNOTATION", builtin_get_annotation },
  { "MAKE-DICT", builtin_make_dict },
  { "DICT-GET", builtin_dict_get },
  { "DICT-SET", builtin_dict_set },
  { "DICT-UNSET", builtin_dict_unset },
  { "DICT-TO-LIST", builtin_dict_to_list },
  { "DICT-HAS-KEY", builtin_dict_has_key },
  { "DICTP", builtin_dictp },
  { "BLOCK", builtin_block},
  { "RETURN-FROM", builtin_return_from},
  { "READ-STRING", builtin_read_string },
};

limo_data *make_globalenv(int argc, char **argv)
{
  limo_data *env = make_env(NULL);
  limo_data *args_start;
  limo_data **args = &args_start;
  int i;

  for (i=0; i<(sizeof builtin_array)/(sizeof builtin_array[0]); ++i)
    setconstq(env, make_sym(builtin_array[i].name), make_builtin(builtin_array[i].f));

  for (i=1; i<argc; ++i) {
    (*args) = make_cons(make_string(argv[i]), NULL);
    args = &CDR(*args);
  }
  (*args) = make_nil();

  setq(env, make_sym("ARGV"), args_start);
  setq(env, make_sym("_TRACE"), make_nil());
  traceplace = var_lookup_place(env, make_sym("_TRACE"));
  setq(env, make_sym("_INTERNED-SYMBOLS"), interned_symbols);
  setq(env, make_sym("_LIMO-PREFIX"), make_string(LIMO_PREFIX));

  number_builtins(env);

  return env;
}
