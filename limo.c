#include "limo.h"
#include <assert.h>
#include <signal.h>
#include <pthread.h>

#ifndef NO_READLINE
#include <readline/readline.h>
#endif //!NO_READLINE

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
limo_data *sym_reader_stream;
limo_data *nil;

pthread_key_t pk_dynamic_vars_key;
pthread_key_t pk_stacktrace_key;
pthread_key_t pk_exception_key;
pthread_key_t pk_ljbuf_key;
pthread_key_t pk_finallystack_key;
pthread_key_t pk_limo_data_next_key;
pthread_key_t pk_stacktrace_free_key;
pthread_key_t pk_flmalloc_key;

void *flmalloc_lists[MAX_FLMALLOC_LISTS];

void *make_limo_data_next = NULL;
void *make_stacktrace_free = NULL;
limo_data *stacktrace;

static void init_pthread_keys(void)
{
  pthread_key_create(&pk_ljbuf_key, NULL);
  pthread_key_create(&pk_stacktrace_key, NULL);
  pthread_key_create(&pk_exception_key, NULL);
  pthread_key_create(&pk_finallystack_key, NULL);
  pthread_key_create(&pk_limo_data_next_key, NULL);
  pthread_key_create(&pk_stacktrace_free_key, NULL);
  pthread_key_create(&pk_dynamic_vars_key, NULL);
  pthread_key_create(&pk_flmalloc_key, NULL);
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
  sym_reader_stream = make_sym("READER-STREAM");
  sym_block      = make_sym("BLOCK");
}

void load_limo_file(char *filename, limo_data *env)
{
  FILE *f;
  reader_stream *rs;
  if (f=fopen(filename, "r")) {
    rs = limo_rs_from_file(f, filename, env);
    while (!limo_eof(rs)) {
      eval(reader(rs), env);
    }
    fclose(f);
  }
  else
    limo_error("LOAD: %s not found", filename);
}

limo_data *limo_init(int argc, char **argv)
{
  limo_data *env;
  limo_data *dynamic_env;
  #ifndef __MINGW32__
  sigset_t sigset;
  #endif

  ////// initializing GC
  GC_all_interior_pointers = HAVE_DISPLACED_POINTERS; // why do I have this? document!

  #ifndef __MINGW32__
  sigemptyset(&sigset);
  sigaddset(&sigset, SIGINT);
  pthread_sigmask(SIG_BLOCK, &sigset, NULL);
  #endif

  // hopefully, the GC-thread gets spawned here
  // and inherits the NO SIGINT
  //GC_use_entire_heap=1;
  GC_init();
  //int free_space_divisor = GC_call_with_alloc_lock(GC_get_free_space_divisor, NULL);
  GC_call_with_alloc_lock(GC_set_free_space_divisor, 1);
#ifdef LIMO_THREADING
  GC_allow_register_threads();
#endif

  assert(GC_thread_is_registered());
#ifndef __MINGW32__
  pthread_sigmask(SIG_UNBLOCK, &sigset, NULL);
#endif
  /////////////////////

  init_pthread_keys();
  pk_limo_data_next_set(&make_limo_data_next);
  pk_stacktrace_free_set(&make_stacktrace_free);
  
  memset(flmalloc_lists, 0, MAX_FLMALLOC_LISTS * sizeof (void *));
  pk_flmalloc_set(flmalloc_lists);

  /////////////////////////////////////////
  // now we can create objects

  init_syms();
  stacktrace = make_nil();
  pk_stacktrace_set(&stacktrace);
  pk_exception_set(nil);
  pk_finallystack_set(NULL);

  dynamic_env = make_env(nil);
  pk_dynamic_vars_set(dynamic_env);
  
  env = make_globalenv(argc, argv);  // needs dynamic_env already
  globalenv = env;

  setq(globalenv, make_sym("_dyn-env"), dynamic_env);

  return globalenv;
}
