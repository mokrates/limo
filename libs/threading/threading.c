#define GC_PTHREADS
#include <limo.h>
#include <pthread.h>

// had to use GC_pthread_create and GC_pthread_join to make this work.
// segfaulted with the usual functions

#define INS_THREADING_BUILTIN(f, name) setq(threading_env, make_sym(name), make_builtin(f))

static limo_data *sym_thread;

typedef struct LIMO_THREADING_THREAD {
  pthread_t pthread;
} limo_threading_thread;

static void *limo_threading_entry_function(void *thread_fun)
{
  eval(make_cons((limo_data *)thread_fun, make_nil()), make_env(nil));
  // TODO return value
}

BUILTIN(builtin_thread_create)
{
  limo_threading_thread *ltt;
  limo_data *ld_ltt;
  limo_data *ld_entry_function;
  limo_data *ld_attr; // todo unused by now

  REQUIRE_ARGC("THREAD-CREATE", 1);
  ld_entry_function = eval(FIRST_ARG, env);
  
  ltt = GC_malloc(sizeof (limo_threading_thread));
  ld_ltt = make_special(sym_thread, ltt);

  // TODO threading-attributes -- OPTIONAL second argument!
  if (GC_pthread_create(&ltt->pthread, NULL, limo_threading_entry_function, ld_entry_function))
    limo_error("Could not create new thread");

  return ld_ltt;
}

BUILTIN(builtin_thread_join)
{
  limo_data *ld_ltt;
  limo_threading_thread *ltt;

  REQUIRE_ARGC("THREAD-JOIN", 1);  
  ltt = get_special(eval(FIRST_ARG, env), sym_thread);
  GC_pthread_join(ltt->pthread, NULL);  // TODO rueckgabewert.

  return make_nil();
}


void limo_init_threading(limo_data *env)
{
  limo_data *threading_env;
  sym_thread = make_sym("THREAD");

  threading_env = make_env(NULL);
  INS_THREADING_BUILTIN(builtin_thread_create, "THREAD-CREATE");
  INS_THREADING_BUILTIN(builtin_thread_join, "THREAD-JOIN");
  
  setq(env, make_sym("_THREADING"), threading_env);
}

