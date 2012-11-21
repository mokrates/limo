#include "limo.h"

limo_data *globalenv;

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
}
