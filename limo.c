#include "limo.h"
#include <signal.h>
#include <execinfo.h>

limo_data *globalenv;

limo_data *stacktrace;

limo_data *sym_env;
limo_data *sym_callerenv;
limo_data *sym_trace;
limo_data *sym_true;
limo_data *sym_stacktrace;
limo_data *sym_underscore;
limo_data *sym_block;
limo_data *nil;

limo_data *traceplace;

void init_syms()
{
  sym_env        = make_sym("_ENV");
  sym_callerenv  = make_sym("_CALLERENV");
  sym_trace      = make_sym("_TRACE");
  sym_stacktrace = make_sym("_STACKTRACE");
  sym_true       = make_sym(":T");
  sym_underscore = make_sym("_");
  sym_block      = make_sym("BLOCK");
  nil            = make_nil();
}

void load_limo_file(char *filename, limo_data *env)
{
  FILE *f;
  reader_stream *rs;
  if (f=fopen(filename, "r")) {
    rs = limo_rs_from_file(f, filename);
    while (!limo_eof(rs)) {
      eval(reader(rs), env);
    }
    fclose(f);
  }
  else
    limo_error("LOAD: %s not found", filename);
}

void limo_repl_sigint(int signum)
{
  limo_error("Keyboard Interrupt");
}

void sigsegv_handler(int sig) {
  void *array[10];
  size_t size;
  
  // get void*'s for all entries on the stack
  size = backtrace(array, 10);
  
  // print out all the frames to stderr
  fprintf(stderr, "Interpreter Error: signal %d:\n", sig);
  backtrace_symbols_fd(array, size, 2);
  exit(1);
}

int main(int argc, char **argv)
{
  limo_data *env;
  reader_stream *rs;
  int marked_const;

  GC_all_interior_pointers = HAVE_DISPLACED_POINTERS; // why do I have this? document!
  GC_init();
  // GC_enable_incremental();   // segfaults :(

  init_syms();
  stacktrace = make_nil();

  env = make_globalenv(argc, argv);
  globalenv = env;

  signal(SIGINT, limo_repl_sigint);
  signal(SIGSEGV, sigsegv_handler);

  if (argc != 2 || strcmp(argv[1], "-n")) {
    rs = limo_rs_from_string("(load (string-concat _limo-prefix \"init.limo\"))");
    if (NULL==try_catch(reader(rs), env)) {
      print_stacktrace(var_lookup(globalenv, sym_stacktrace, &marked_const));
      writer(exception);
      printf("\n");
      exit(1);
    }
  }
  
  rs = limo_rs_make_readline();

  while (!limo_eof(rs)) { // REPL
    //    jmp_buf jb;
    limo_data *ld;
    ljbuf = (sigjmp_buf *)GC_malloc(sizeof (sigjmp_buf));
    if (sigsetjmp(*ljbuf, 1)) {
      printf("\nUNHANDLED EXCEPTION CAUGHT\n");
      if (exception) {
	rs = limo_rs_make_readline();
	print_stacktrace(var_lookup(globalenv, sym_stacktrace, &marked_const));
	stacktrace = make_nil();
	writer(exception);
	printf("\n");
      }
      signal(SIGINT, limo_repl_sigint);
      exception=NULL;
    }
    else {
      ld=reader(rs);
      ld = eval(ld, env);
      printf("-> ");
      writer(ld);
      printf("\n");
      setq(globalenv, sym_underscore, ld);
    }
  }

  return 0;
}
