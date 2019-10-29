#include <limo.h>

#define INSBUILTIN(f, name) setq(env, make_sym(name), make_builtin(f))

BUILTIN(builtin_string_nth)
{
  char buf[3]="\0\0";
  limo_data *n, *str;

  if (list_length(arglist) != 3)
    limo_error("(string-nth N STRING) wrong number of arguments: given %i", list_length(arglist));

  n = eval(FIRST_ARG, env);
  str = eval(SECOND_ARG, env);
  if (n->type != limo_TYPE_GMPQ ||
      str->type != limo_TYPE_STRING)
    limo_error("(string-nth N STRING)");

  *buf = str->data.d_string[(int)mpq_get_d(*n->data.d_mpq)];
  return make_string(buf);
}

BUILTIN(builtin_string_length)
{
  limo_data *n, *str;

  if (list_length(arglist) != 2)
    limo_error("(string-length STRING)");

  str = eval(FIRST_ARG, env);
  if (str->type != limo_TYPE_STRING)
    limo_error("(string-length STRING)");

  return make_number_from_long_long(strlen(str->data.d_string));
}

BUILTIN(builtin_stringp)
{
  if (list_length(arglist) != 2)
    limo_error("(stringp VALUE)");

  if (eval(FIRST_ARG, env)->type == limo_TYPE_STRING)
    return sym_true;
  else
    return make_nil();
}

void limo_inline_init_string_builtins(limo_data *env)
{
  INSBUILTIN(builtin_string_nth, "STRING-NTH");
  INSBUILTIN(builtin_string_length, "STRING-LENGTH");
  INSBUILTIN(builtin_stringp, "STRINGP");
}

#ifndef LIMO_MOD_INLINE
void limo_dll_init(limo_data *env) 
{
  limo_inline_init_string_builtins(env);
}
#endif
