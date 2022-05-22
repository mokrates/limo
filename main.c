#include "limo.h"
#include <signal.h>

#ifndef NO_EXECINFO
#include <execinfo.h>
#endif // !NO_EXECINFO

#ifndef NO_READLINE
#include <readline/readline.h>
#endif //!NO_READLINE


#ifndef NO_EXECINFO
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
#endif //!NO_EXECINFO

void limo_repl_sigint(int signum)
{
#ifndef NO_READLINE
  if (rl_readline_state & RL_STATE_SIGHANDLER) {
    limo_error("Keyboard Interrupt");
  }
  else
#endif //!NO_READLINE
    
    limo_register |= LR_SIGINT;
}

int main(int argc, char **argv)
{
  limo_data *env;
  reader_stream *rs;  

  env = limo_init(argc, argv);

  signal(SIGINT, limo_repl_sigint);

#ifndef NO_EXECINFO
  signal(SIGSEGV, sigsegv_handler);
#endif //!NO_EXECINFO
  
#ifdef LIMO_MAKE_EXECUTABLE
  rs = limo_rs_from_string(limo_program_cstr);
  while (!limo_eof(rs)) {
    if (NULL==try_catch(reader(rs), env)) {
      print_stacktrace(stacktrace);
      writer(pk_exception_get());
      printf("\n");
      exit(1);
    }
  }
#else

  if (argc != 2 || strcmp(argv[1], "-n")) {
    rs = limo_rs_from_string("(load (string-concat _limo-prefix \"init.limo\"))", env);
    if (NULL==try_catch(reader(rs), env)) {
      print_stacktrace(stacktrace);
      writer(pk_exception_get());
      printf("\n");
      exit(1);
    }
  }

#ifndef NO_READLINE
  rs = limo_rs_make_readline(env);
#else //!NO_READLINE
  rs = limo_rs_from_file(stdin, "STDIN", env);
#endif //!NO_READLINE

  while (!limo_eof(rs)) { // REPL
    limo_data *ld;
    sigjmp_buf ljbuf;
    pk_ljbuf_set(&ljbuf);
    if (sigsetjmp(*pk_ljbuf_get(), 1)) {
      printf("\nUNHANDLED EXCEPTION CAUGHT\n");
      if (pk_exception_get()) {
        signal(SIGINT, limo_repl_sigint);

#ifndef NO_READLINE
        rl_readline_state &= ~RL_STATE_SIGHANDLER;
        rl_reset_after_signal();
    	rs = limo_rs_make_readline(env);
#else // NO_READLINE
	rs = limo_rs_from_file(stdin, "STDIN", env);
#endif //!NO_READLINE

    	print_stacktrace(stacktrace);
        stacktrace = nil;
    	writer(pk_exception_get());
    	printf("\n");
      }
    }
    else {
      ld=reader(rs);
      ld = eval(ld, env);
      printf("-> ");
      writer(ld);
      printf("\n");
      setq(globalenv, sym_underscore, ld);
    }

#endif  // !LIMO_MAKE_EXECUTABLE
  }

  //GC_reachable_here(dynamic_env);  // I have put it into the globalenv to make it visible to the GC.

  return 0;
}

