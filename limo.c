#include "limo.h"

limo_data *globalenv;

limo_data *sym_env;
limo_data *sym_callerenv;
limo_data *sym_trace;
limo_data *sym_true;

void init_syms()
{
  sym_env       = make_sym("_ENV");
  sym_callerenv = make_sym("_CALLERENV");
  sym_trace     = make_sym("_TRACE");
  sym_true      = make_sym(":T");
}

void load_limo_file(char *filename, limo_data *env)
{
  FILE *f;
  if (f=fopen(filename, "r")) {
    while (!feof(f)) {
      eval(reader(limo_rs_from_file(f)), env);
    }
    fclose(f);
  }
  else
    limo_error("LOAD: %s not found", filename);
}

int main(int argc, char **argv)
{
  limo_data *env;
  reader_stream *rs;

  GC_init();

  init_syms();

  env = make_globalenv(argc, argv);
  globalenv = env;

  load_limo_file("init.limo", env);

  rs = limo_rs_make_readline();

  while (!limo_eof(rs)) { // REPL
    //    jmp_buf jb;
    limo_data *ld;
    ljbuf = (jmp_buf *)GC_malloc(sizeof (jmp_buf));
    if (setjmp(*ljbuf)) {
      printf("\nUNHANDLED EXCEPTION CAUGHT\n");
      if (exception) {
	writer(exception);
	printf("\n");
      }
      exception=NULL;
    }
    else {
      ld=reader(rs);
      ld = eval(ld, env);
      printf("-> ");
      writer(ld);
      printf("\n");
    }
  }

  return 0;
}
