#include "limo.h"

struct { char *name; limo_builtin f; }
  builtin_array[] = {
                     { "LIST", builtin_list },
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
                     { "DCONS", builtin_dcons },
                     { "EQ", builtin_eq},
                     { "WRITE", builtin_write},
                     { "WRITE-TO-LIST", builtin_write_to_list},
                     { "TRY", builtin_try },
                     { "FINALLY", builtin_finally},
                     { "THROW", builtin_throw },
                     { "LOAD", builtin_load},
                     { "MOD-ISINLINE", builtin_mod_isinline },
                     { "MOD-LOADINLINE", builtin_mod_loadinline },
                     { "LOADDLL", builtin_loaddll },
                     { "GCENABLE", builtin_gc_enable },
                     { "GCDISABLE", builtin_gc_disable },
                     { "GCCOLLECT", builtin_gc_collect},
                     { "OPTDISABLE", builtin_opt_disable },
                     { "GCSETMAX", builtin_gc_setmax},
                     { "ENV-EXTRACT", builtin_env_extract },
                     { "ENVP", builtin_envp },
                     { "USLEEP", builtin_usleep },
                     { "STRING-CONCAT", builtin_string_concat },
                     { "MAKE-SYM", builtin_make_sym },
                     { "MAKE-SYM-UNINTERNED", builtin_make_sym_uninterned },
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
                     { "SYMBOLP", builtin_symbolp},
                     { "SYMBOL-TO-STRING", builtin_symbol_to_string},
                     { "FREEZEQ", builtin_freezeq },
                     { "ADDRESS-OF", builtin_address_of },
                     { "MAKE-ENV", builtin_make_env },
                     { "ENV-SETQ", builtin_env_setq },
                     { "ENV-SETF", builtin_env_setf },
                     { "ENV-GETQ", builtin_env_getq },
                     { "ENV-CURRENT", builtin_env_current },
                     { "GET-DYNAMIC-ENV", builtin_get_dynamic_env },
                     { "_WITH-DYNAMIC-ENV", builtin_with_dynamic_env },
                     { "SET-FINALIZER", builtin_set_finalizer },
                     { "SYSTEM", builtin_system},
                     { "TIME", builtin_time}
  };

struct { char *name; limo_builtinfun f; }
  builtinfun_array[] = {
                        { "CONS", builtin_cons},
                        { "CONSP", builtin_consp},
                        { "CAR", builtin_car},
                        { "CDR", builtin_cdr},
                        { "SYM-NO-OPT", builtin_sym_no_opt },
                        { "READER-STREAM-GETC", builtin_reader_stream_getc },
                        { "READER-STREAM-UNGETC", builtin_reader_stream_ungetc },
                        { "READER-STREAM-EOF", builtin_reader_stream_eof },
                        { "READ", builtin_read },
                        { "EXIT", builtin_exit },
  };



limo_data *make_globalenv(int argc, char **argv)
{
  limo_data *env = make_env(make_nil());
  limo_data *args_start;
  limo_data **args = &args_start;
  int i;

  for (i=0; i<(sizeof builtin_array)/(sizeof builtin_array[0]); ++i)
    setconstq(env, make_sym(builtin_array[i].name), make_builtin(builtin_array[i].f, builtin_array[i].name));

  for (i=0; i<(sizeof builtinfun_array)/(sizeof builtinfun_array[0]); ++i)
    setconstq(env, make_sym(builtinfun_array[i].name), make_builtinfun(builtinfun_array[i].f, builtinfun_array[i].name));

  for (i=1; i<argc; ++i) {
    (*args) = make_cons(make_string(argv[i]), NULL);
    args = &CDR(*args);
  }
  (*args) = make_nil();

  setq(env, make_sym("ARGV"), args_start);
  setq(env, make_sym("_INTERNED-SYMBOLS"), interned_symbols);
  setq(env, make_sym("_LIMO-PREFIX"), make_string(LIMO_PREFIX));
#if defined(ANDROID)
  setq(env, make_sym("_ARCH"), make_string("ANDROID"));
#elif defined(__CYGWIN__)
  setq(env, make_sym("_ARCH"), make_string("CYGWIN"));
#elif defined(__MINGW32__)
  setq(env, make_sym("_ARCH"), make_string("MINGW"));
#else
  setq(env, make_sym("_ARCH"), make_string("LINUX"));
#endif
  
  number_builtins(env);

  setq(pk_dynamic_vars_get(), make_sym("_FLOAT-WRITER-FORMAT"), make_string("%g"));

  return env;
}
