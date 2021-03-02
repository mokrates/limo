#include <limo.h>
#include <errno.h>

#define INSBUILTIN(f, name) setq(env, make_sym(name), make_builtin(f))

limo_data *sym_file;

BUILTIN(builtin_file_open)
{
  FILE *f;
  limo_data *filename, *mode;

  if (list_length(arglist) < 3)
    limo_error("(FILE-OPEN filename mode)");

  filename = eval(FIRST_ARG, env);
  mode = eval(SECOND_ARG, env);
  if (filename->type != limo_TYPE_STRING ||
      mode->type != limo_TYPE_STRING)
    limo_error("(FILE-OPEN filename mode)");

  f = fopen(filename->data.d_string, mode->data.d_string);
  if (f == NULL) {
    char buf[512];
    strerror_r(errno, buf, 512);
    throw(make_cons(sym_file, make_string(buf)));
  }

  return make_special(sym_file, f);
}

BUILTIN(builtin_file_fdopen)
{
  FILE *f;
  limo_data *fd, *mode;

  REQUIRE_ARGC("FILE-FDOPEN", 2);
  
  fd = eval(FIRST_ARG, env);
  mode = eval(SECOND_ARG, env);
  REQUIRE_TYPE("FILE-FDOPEN", fd, limo_TYPE_GMPQ);
  REQUIRE_TYPE("FILE-FDOPEN", mode, limo_TYPE_STRING);

  f = fdopen(GETINTFROMMPQ(fd), mode->data.d_string);
  if (f == NULL) {
    char buf[512];
    strerror_r(errno, buf, 512);
    throw(make_cons(sym_file, make_string(buf)));
  }

  return make_special(sym_file, f);
}

BUILTIN(builtin_file_close)
{
  FILE *f;
  REQUIRE_ARGC("FILE-CLOSE", 1);
  f=get_special(eval(FIRST_ARG, env),sym_file);
  fclose(f);
  return make_nil();
}

BUILTIN(builtin_file_getc)
{
  FILE *f;
  char buf[3]="\0\0";
  limo_data *ld;

  if (list_length(arglist)<2)
    limo_error("file_getc FILE");

  f=get_special(eval(FIRST_ARG, env),sym_file);
  *buf = fgetc(f);
  if (feof(f))
    return make_string("");
  ld = make_string(buf);
  ld->hash=1;
  return ld;
}

BUILTIN(builtin_file_write)
{
  FILE *f;
  limo_data *str;
  size_t ret;

  if (list_length(arglist) < 3)
    limo_error("(file-write FILE STRING)");

  f=get_special(eval(FIRST_ARG, env),sym_file);
  str = eval(SECOND_ARG, env);
  if (str->type !=limo_TYPE_STRING)
    limo_error("(file-write FILE STRING)");

  ret = fwrite(str->data.d_string, str->string_length, 1, f);
  return make_number_from_long_long(ret);
}

BUILTIN(builtin_file_flush)
{
  FILE *f;

  REQUIRE_ARGC("FILE-FLUSH", 1);
  f=get_special(eval(FIRST_ARG, env),sym_file);
  fflush(f);
  return nil;
}

BUILTIN(builtin_file_seek)
{
  limo_data *ldpos = eval(SECOND_ARG, env);
  limo_data *ldwhence = eval(THIRD_ARG, env);
  REQUIRE_ARGC("FILE-SEEK", 3);
  REQUIRE_TYPE("FILE-SEEK", ldpos, limo_TYPE_GMPQ);
  REQUIRE_TYPE("FILE-SEEK", ldwhence, limo_TYPE_GMPQ);
  fseek((FILE *)get_special(eval(FIRST_ARG, env), sym_file),
	GETINTFROMMPQ(ldpos),
	GETINTFROMMPQ(ldwhence));
  return nil;
}

BUILTIN(builtin_file_tell)
{
  REQUIRE_ARGC("FILE-TELL", 1);
  return make_number_from_long_long(ftell((FILE *)get_special(eval(FIRST_ARG, env), sym_file)));
}

void limo_init_file(limo_data *env)
{
  sym_file = make_sym("FILE");
  INSBUILTIN(builtin_file_close, "FILE-CLOSE");
  INSBUILTIN(builtin_file_flush, "FILE-FLUSH");
  INSBUILTIN(builtin_file_fdopen, "FILE-FDOPEN");
  INSBUILTIN(builtin_file_getc, "FILE-GETC");
  INSBUILTIN(builtin_file_open, "FILE-OPEN");
  INSBUILTIN(builtin_file_seek, "FILE-SEEK");
  INSBUILTIN(builtin_file_tell, "FILE-TELL");
  INSBUILTIN(builtin_file_write, "FILE-WRITE");
  setq(env, make_sym("FILE-SEEK-SET"), make_number_from_long_long(SEEK_SET));
  setq(env, make_sym("FILE-SEEK-CUR"), make_number_from_long_long(SEEK_CUR));
  setq(env, make_sym("FILE-SEEK-END"), make_number_from_long_long(SEEK_END));
}

