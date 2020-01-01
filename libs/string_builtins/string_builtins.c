#include <limo.h>

#define INSBUILTIN(f, name) setq(env, make_sym(name), make_builtin(f))

BUILTIN(builtin_string_nth)
{
  char buf[3]="\0\0";
  limo_data *n, *str;
  int i;

  if (list_length(arglist) != 3)
    limo_error("(string-nth N STRING) wrong number of arguments: given %i", list_length(arglist));

  n = eval(FIRST_ARG, env);
  str = eval(SECOND_ARG, env);
  if (n->type != limo_TYPE_GMPQ ||
      str->type != limo_TYPE_STRING)
    limo_error("(string-nth N STRING)");

  i = (int)mpq_get_d(*n->data.d_mpq);
  if (i<0 || i>str->hash)
    limo_error("string-nth: out of bounds");
  *buf = str->data.d_string[i];
  return make_string(buf);
}

BUILTIN(builtin_string_length)
{
  limo_data *str;

  if (list_length(arglist) != 2)
    limo_error("(string-length STRING)");

  str = eval(FIRST_ARG, env);
  if (str->type != limo_TYPE_STRING)
    limo_error("(string-length STRING)");

  return make_number_from_long_long(str->hash);
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

BUILTIN(builtin_ord)
{
  limo_data *ld;
  REQUIRE_ARGC("ORD", 1);
  ld = eval(FIRST_ARG, env);
  if (ld->type != limo_TYPE_STRING)
    limo_error("(ORD string)");
  return make_number_from_long_long((unsigned char)ld->data.d_string[0]);
}

BUILTIN(builtin_chr)
{
  char buf[3]="\0\0";
  limo_data *n, *str;
  int i;

  REQUIRE_ARGC("CHR", 1);
  
  n = eval(FIRST_ARG, env);
  if (n->type != limo_TYPE_GMPQ)
    limo_error("(CHR number)");

  i = (int)mpq_get_d(*n->data.d_mpq);
  *buf = (char)i;
  str = make_string(buf);
  str->hash=1;
  return str;
}

void limo_init_string_builtins(limo_data *env)
{
  INSBUILTIN(builtin_string_nth, "STRING-NTH");
  INSBUILTIN(builtin_string_length, "STRING-LENGTH");
  INSBUILTIN(builtin_stringp, "STRINGP");
  INSBUILTIN(builtin_ord, "ORD");
  INSBUILTIN(builtin_chr, "CHR");
}
