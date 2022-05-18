#include <limo.h>
#include <errno.h>
#include <dirent.h>
#include <sys/types.h>

#define INSBUILTIN(f, name) setq(env, make_sym(name), make_builtin(f))
#define INSBUILTINFUN(f, name) setq(env, make_sym(name), make_builtinfun(f))

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

  f = fopen(filename->d_string, mode->d_string);
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

  f = fdopen(GETINTFROMMPQ(fd), mode->d_string);
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

BUILTINFUN(builtin_file_eof)
{
  FILE *f;
  REQUIRE_ARGC_FUN("FILE-EOF", 1);
  f = get_special(argv[0], sym_file);
  if (feof(f))
    return sym_true;
  else
    return nil;
}

BUILTINFUN(builtin_file_read)
{
  FILE *f;
  size_t len, rlen;
  limo_data *res;
  
  REQUIRE_ARGC_FUN("FILE-READ", 2);
  f = get_special(argv[0], sym_file);
  len = GETINTFROMMPQ(argv[1]);
  res = make_limo_data();
  res->type = limo_TYPE_STRING;
  res->d_string = GC_malloc_atomic(len+1);
  rlen = fread(res->d_string, 1, len, f);
  if (rlen < len)
    res->d_string = GC_realloc(res->d_string, rlen+1);
  res->d_string[rlen] = '\0';
  res->string_length = rlen;
  return res;
}

BUILTIN(builtin_file_getc)
{
  FILE *f;
  limo_data *ld;

  if (list_length(arglist)<2)
    limo_error("file_getc FILE");

  f=get_special(eval(FIRST_ARG, env),sym_file);
  ld = make_limo_data();
  ld->type = limo_TYPE_STRING;
  ld->d_string = (char *)&ld->cdr;
  ld->d_string[0] = fgetc(f);
  ld->d_string[1] = '\0';
  if (feof(f)) {
    ld->d_string[0] = '\0';
    ld->string_length = 0;
  }
  else
    ld->string_length = 1;
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

  ret = fwrite(str->d_string, str->string_length, 1, f);
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

BUILTINFUN(builtin_file_readdir)
{
  DIR *d;
  struct dirent *de;
  limo_data *ld_res, ld_de;
  char f_type[2] = "\0\0";

  REQUIRE_ARGC_FUN("FILE-READDIR", 1);
  REQUIRE_TYPE("FILE-READDIR", argv[0], limo_TYPE_STRING);
  d = opendir(argv[0]->d_string);
  if (!d)
    limo_error_errno(make_sym("READDIR-ERROR"));

  ld_res = nil;
  errno=0;
  while (de = readdir(d)) {
    switch (de->d_type) {
    case DT_BLK: f_type[0] = 'b'; break;
    case DT_CHR: f_type[0] = 'c'; break;
    case DT_DIR: f_type[0] = 'd'; break;
    case DT_FIFO: f_type[0] = 'p'; break;
    case DT_LNK: f_type[0] = 'l'; break;
    case DT_REG: f_type[0] = '-'; break;
    case DT_SOCK: f_type[0] = 's'; break;
    default: f_type[0] = '?';
    }
    ld_res = make_cons(make_cons(make_string(de->d_name),
                                 make_cons(make_string(f_type), nil)),
                       ld_res);
  }
  if (errno)
    limo_error_errno(make_sym("READDIR-ERROR"));

  closedir(d);

  return ld_res;
}

void limo_init_file(limo_data *env)
{
  sym_file = make_sym("FILE");
  INSBUILTIN(builtin_file_close, "FILE-CLOSE");
  INSBUILTIN(builtin_file_flush, "FILE-FLUSH");
  INSBUILTIN(builtin_file_fdopen, "FILE-FDOPEN");
  INSBUILTIN(builtin_file_getc, "FILE-GETC");
  INSBUILTINFUN(builtin_file_eof, "FILE-EOF");
  INSBUILTINFUN(builtin_file_read, "FILE-READ");
  INSBUILTIN(builtin_file_open, "FILE-OPEN");
  INSBUILTIN(builtin_file_seek, "FILE-SEEK");
  INSBUILTIN(builtin_file_tell, "FILE-TELL");
  INSBUILTIN(builtin_file_write, "FILE-WRITE");

  INSBUILTINFUN(builtin_file_readdir, "FILE-READDIR");
  setq(env, make_sym("FILE-SEEK-SET"), make_number_from_long_long(SEEK_SET));
  setq(env, make_sym("FILE-SEEK-CUR"), make_number_from_long_long(SEEK_CUR));
  setq(env, make_sym("FILE-SEEK-END"), make_number_from_long_long(SEEK_END));
}

