#include "limo.h"
#include <assert.h>
#include <signal.h>
#include <execinfo.h>
#include <pthread.h>
#include <readline/readline.h>

unsigned long long limo_register;

// have to be global, are collected, if not.
limo_data *globalenv;

limo_data *sym_env;
limo_data *sym_callerenv;
limo_data *sym_trace;
limo_data *sym_true;
limo_data *sym_stacktrace;
limo_data *sym_underscore;
limo_data *sym_block;
limo_data *nil;

pthread_key_t pk_dynamic_vars_key;
pthread_key_t pk_stacktrace_key;
pthread_key_t pk_exception_key;
pthread_key_t pk_ljbuf_key;
pthread_key_t pk_finallystack_key;
pthread_key_t pk_limo_data_next_key;
pthread_key_t pk_cons_next_key;
pthread_key_t pk_dict_next_key;
pthread_key_t pk_gmpq_next_key;

void *make_limo_data_next = NULL;
void *make_cons_next = NULL;
void *make_dict_next = NULL;
void *make_gmpq_next = NULL;

static void init_pthread_keys(void)
{
  pthread_key_create(&pk_ljbuf_key, NULL);
  pthread_key_create(&pk_stacktrace_key, NULL);
  pthread_key_create(&pk_exception_key, NULL);
  pthread_key_create(&pk_finallystack_key, NULL);
  pthread_key_create(&pk_limo_data_next_key, NULL);
  pthread_key_create(&pk_cons_next_key, NULL);
  pthread_key_create(&pk_dict_next_key, NULL);
  pthread_key_create(&pk_gmpq_next_key, NULL);  
  pthread_key_create(&pk_dynamic_vars_key, NULL);
}

static void init_syms()
{
  nil            = make_nil();
  sym_env        = make_sym("_ENV");
  sym_callerenv  = make_sym("_CALLERENV");
  sym_trace      = make_sym("_TRACE");
  sym_stacktrace = make_sym("_STACKTRACE");
  sym_true       = make_sym(":T");
  sym_underscore = make_sym("_");
  sym_block      = make_sym("BLOCK");
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
  if (rl_readline_state & RL_STATE_SIGHANDLER) {
    limo_error("Keybord Interrupt");
  }
  else
    limo_register |= LR_SIGINT;
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
  limo_data *dynamic_env;  
  reader_stream *rs;
  int marked_const;
  sigset_t sigset;

  ////// initializing GC
  GC_all_interior_pointers = HAVE_DISPLACED_POINTERS; // why do I have this? document!

  sigemptyset(&sigset);
  sigaddset(&sigset, SIGINT);
  pthread_sigmask(SIG_BLOCK, &sigset, NULL);

  // hopefully, the GC-thread gets spawned here
  // and inherits the NO SIGINT
  GC_use_entire_heap=1;
  GC_init();
  //int free_space_divisor = GC_call_with_alloc_lock(GC_get_free_space_divisor, NULL);
  GC_call_with_alloc_lock(GC_set_free_space_divisor, 1);
  GC_allow_register_threads();

  assert(GC_thread_is_registered());

  pthread_sigmask(SIG_UNBLOCK, &sigset, NULL);
  /////////////////////

  init_pthread_keys();
  pk_limo_data_next_set(&make_limo_data_next);
  pk_cons_next_set(&make_cons_next);
  pk_dict_next_set(&make_dict_next);
  pk_gmpq_next_set(&make_gmpq_next);

  init_syms();
  pk_stacktrace_set(nil);
  pk_exception_set(nil);
  pk_finallystack_set(NULL);
 
  env = make_globalenv(argc, argv);
  globalenv = env;

  dynamic_env = make_env(nil);
  pk_dynamic_vars_set(dynamic_env);
   
  signal(SIGINT, limo_repl_sigint);
  signal(SIGSEGV, sigsegv_handler);

#ifdef LIMO_MAKE_EXECUTABLE
  rs = limo_rs_from_string(limo_program_cstr);
  while (!limo_eof(rs)) {
    if (NULL==try_catch(reader(rs), env)) {
      print_stacktrace(pk_stacktrace_get());
      writer(pk_exception_get());
      printf("\n");
      exit(1);
    }
  }
#else

  if (argc != 2 || strcmp(argv[1], "-n")) {
    rs = limo_rs_from_string("(load (string-concat _limo-prefix \"init.limo\"))");
    if (NULL==try_catch(reader(rs), env)) {
      print_stacktrace(pk_stacktrace_get());
      writer(pk_exception_get());
      printf("\n");
      exit(1);
    }
  }

  rs = limo_rs_make_readline();

  while (!limo_eof(rs)) { // REPL
    limo_data *ld;
    sigjmp_buf ljbuf;
    pk_ljbuf_set(&ljbuf);
    if (sigsetjmp(*pk_ljbuf_get(), 1)) {
      printf("\nUNHANDLED EXCEPTION CAUGHT\n");
      if (pk_exception_get()) {
        signal(SIGINT, limo_repl_sigint);
        rl_readline_state &= ~RL_STATE_SIGHANDLER;
        rl_reset_after_signal();
    	rs = limo_rs_make_readline();
    	print_stacktrace(pk_stacktrace_get());
    	pk_stacktrace_set(nil);
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

#endif
  }

  GC_reachable_here(dynamic_env);
  
  return 0;
}
