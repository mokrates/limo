#include <limo.h>

#define INSBUILTIN(f, name) setq(env, make_sym(name), make_builtin(f))
#define INSBUILTINFUN(f, name) setq(env, make_sym(name), make_builtinfun(f))

BUILTIN(builtin_string_nth)
{
  char buf[3]="\0\0";
  limo_data *n, *str, *res;
  int i;

  if (list_length(arglist) != 3)
    limo_error("(string-nth N STRING) wrong number of arguments: given %i", list_length(arglist));

  n = eval(FIRST_ARG, env);
  str = eval(SECOND_ARG, env);
  if (n->type != limo_TYPE_GMPQ ||
      str->type != limo_TYPE_STRING)
    limo_error("(string-nth N STRING)");

  i = (int)mpq_get_d(*n->d_mpq);
  if (i<0 || i>=str->string_length)
    limo_error("string-nth: out of bounds");
  res = make_limo_data();
  res->string_length = 1;
  res->type = limo_TYPE_STRING;
  res->d_string = (char *)&(res->cdr);
  res->d_string[1] = '\0';
  res->d_string[0] = str->d_string[i];
  return res;
}

BUILTIN(builtin_string_length)
{
  limo_data *str;

  if (list_length(arglist) != 2)
    limo_error("(string-length STRING)");

  str = eval(FIRST_ARG, env);
  if (str->type != limo_TYPE_STRING)
    limo_error("(string-length STRING)");

  return make_rational_from_long_long(str->string_length);
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
  return make_rational_from_long_long((unsigned char)ld->d_string[0]);
}

BUILTIN(builtin_chr)
{
  limo_data *n, *str;
  int i;

  REQUIRE_ARGC("CHR", 1);

  n = eval(FIRST_ARG, env);
  if (n->type != limo_TYPE_GMPQ)
    limo_error("(CHR number)");

  i = (int)mpq_get_d(*n->d_mpq);
  str = make_limo_data();
  str->type = limo_TYPE_STRING;
  str->d_string = (char *)&str->cdr;
  str->d_string[0] = (unsigned char)i;
  str->d_string[1] = '\0';
  str->string_length=1;
  return str;
}


// stolen from https://stackoverflow.com/questions/32936646/getting-the-string-length-on-utf-8-in-c
BUILTIN(builtin_string_utf8_length) {
  limo_data *str;
  char *s;
  size_t count = 0;

  if (list_length(arglist) != 2)
    limo_error("(string-length STRING)");

  str = eval(FIRST_ARG, env);
  if (str->type != limo_TYPE_STRING)
    limo_error("(string-length STRING)");

  s = str->d_string;
  while (*s) {
    count += (*s++ & 0xC0) != 0x80;
  }
  return make_rational_from_long_long(count);
}

BUILTINFUN(builtin_list_to_string)
{
  limo_data *ld, *res;
  unsigned int total_length = 0, pos = 0;
  
  REQUIRE_ARGC_FUN("LIST-TO-STRING", 1);
  for (ld=argv[0]; !is_nil(ld); ld=CDR(ld))
    if (CAR(ld)->type == limo_TYPE_STRING)
      total_length += CAR(ld)->string_length;
    else
      limo_error("list-to-string expects a list of strings");

  res = make_limo_data();
  res->type = limo_TYPE_STRING;
  res->d_string = GC_malloc_atomic(total_length+1);
  res->string_length = total_length;
  for (ld=argv[0]; !is_nil(ld); ld=CDR(ld)) {
    memcpy(res->d_string + pos, CAR(ld)->d_string, CAR(ld)->string_length);
    pos+=CAR(ld)->string_length;
  }
  res->d_string[pos]='\0';
  return res;
}

void limo_init_string_builtins(limo_data *env)
{
  INSBUILTIN(builtin_string_nth, "STRING-NTH");
  INSBUILTIN(builtin_string_length, "STRING-LENGTH");
  INSBUILTIN(builtin_stringp, "STRINGP");
  INSBUILTIN(builtin_ord, "ORD");
  INSBUILTIN(builtin_chr, "CHR");
  INSBUILTIN(builtin_string_utf8_length, "STRING-UTF8-LENGTH");
  INSBUILTINFUN(builtin_list_to_string, "LIST-TO-STRING");
}
