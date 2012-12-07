#include <limo.h>

#define INSBUILTIN(f, name) setq(env, make_sym(name), make_builtin(f))

limo_data *sym_file;

BUILTIN(builtin_file_open)
{
  FILE *f;
  limo_data *filename, *mode;

  if (list_length(arglist) < 3)
    limo_error("(file-open FILENAME MODE)");

  filename = eval(FIRST_ARG, env);
  mode = eval(SECOND_ARG, env);
  if (filename->type != limo_TYPE_STRING ||
      mode->type != limo_TYPE_STRING)
    limo_error("(file-open FILENAME MODE)");

  f = fopen(filename->data.d_string, mode->data.d_string);
  if (f == NULL)
    throw(sym_file);

  return make_special(sym_file, f);
}

BUILTIN(builtin_file_close)
{
  FILE *f;

  if (list_length(arglist)<2)
    limo_error("file_close FILE");

  f=get_special(eval(FIRST_ARG, env),sym_file);
  fclose(f);
  return make_nil();
}

BUILTIN(builtin_file_getc)
{
  FILE *f;
  char buf[3]="\0\0";

  if (list_length(arglist)<2)
    limo_error("file_getc FILE");

  f=get_special(eval(FIRST_ARG, env),sym_file);
  *buf = fgetc(f);
  if (feof(f))
    return make_string("");
  return make_string(buf);
}

BUILTIN(builtin_file_print)
{
  FILE *f;
  limo_data *str;

  if (list_length(arglist) < 3)
    limo_error("(file-print FILE STRING)");

  f=get_special(eval(FIRST_ARG, env),sym_file);
  str = eval(SECOND_ARG, env);
  if (str->type !=limo_TYPE_STRING)
    limo_error("(file-print FILE STRING)");

  fprintf(f, "%s", str->data.d_string);
  return make_nil();
}

void limo_dll_init(limo_data *env) 
{
  sym_file = make_sym("FILE");
  INSBUILTIN(builtin_file_open, "FILE-OPEN");
  INSBUILTIN(builtin_file_close, "FILE-CLOSE");
  INSBUILTIN(builtin_file_getc, "FILE-GETC");
  INSBUILTIN(builtin_file_print, "FILE-PRINT");
}
