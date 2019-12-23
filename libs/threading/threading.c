#define GC_THREADS
#include <pthread.h>
#include <limo.h>

// had to use GC_pthread_create and GC_pthread_join to make this work.
// segfaulted with the usual functions

#define INS_THREADING_BUILTIN(f, name) setq(threading_env, make_sym(name), make_builtin(f))

static limo_data *sym_thread;
static limo_data *sym_mutex;

static limo_data *sym_mutex_attr_fast;
static limo_data *sym_mutex_attr_rec;
static limo_data *sym_mutex_attr_errchk;

typedef struct LIMO_THREADING_THREAD {
  pthread_t pthread;
} limo_threading_thread;

typedef struct LIMO_THREADING_MUTEX {
  pthread_mutex_t mutex;
} limo_threading_mutex;

static void *limo_threading_entry_function(void *thread_fun)
{
  eval(make_cons((limo_data *)thread_fun, nil), make_env(nil));
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

BUILTIN(builtin_mutex_create)
{
  limo_data *ld_mutex_type;
  limo_threading_mutex *lt_mutex;
  pthread_mutexattr_t pma;

  pthread_mutexattr_init(&pma);
  lt_mutex = (limo_threading_mutex *)GC_malloc(sizeof (limo_threading_mutex));
  if (list_length(arglist)>1) {
    ld_mutex_type = eval(FIRST_ARG, env);
    if (limo_equals(ld_mutex_type, sym_mutex_attr_fast))
      pthread_mutexattr_settype(&pma, PTHREAD_MUTEX_TIMED_NP);
    else if (limo_equals(ld_mutex_type, sym_mutex_attr_rec))
      pthread_mutexattr_settype(&pma, PTHREAD_MUTEX_RECURSIVE_NP);
    else if (limo_equals(ld_mutex_type, sym_mutex_attr_errchk))
      pthread_mutexattr_settype(&pma, PTHREAD_MUTEX_ERRORCHECK_NP);
    else
      limo_error("(MUTEX-CREATE [ :FAST | :RECURSIVE | :ERRORCHECK ])");
  }
  else
    pthread_mutexattr_settype(&pma, PTHREAD_MUTEX_TIMED_NP);
  pthread_mutex_init(&lt_mutex->mutex, &pma);

  return make_special(sym_mutex, lt_mutex);
}

BUILTIN(builtin_mutex_lock)
{
  limo_data *ld_mutex;
  limo_threading_mutex *ltm;

  REQUIRE_ARGC("MUTEX-LOCK", 1);
  ld_mutex = eval(FIRST_ARG, env);
  ltm = get_special(ld_mutex, sym_mutex);

  if (pthread_mutex_lock(&ltm->mutex))
    limo_error("Deadlock Condition");

  return ld_mutex;  // not nil value
}

BUILTIN(builtin_mutex_trylock)
{
  limo_data *ld_mutex;
  limo_threading_mutex *ltm;
  int ret;

  REQUIRE_ARGC("MUTEX-LOCK", 1);
  ld_mutex = eval(FIRST_ARG, env);
  ltm = get_special(ld_mutex, sym_mutex);

  if (pthread_mutex_trylock(&ltm->mutex))
    return nil;     // not successful

  return ld_mutex;  // not nil value
  
}

BUILTIN(builtin_mutex_unlock)
{
  limo_data *ld_mutex;
  limo_threading_mutex *ltm;

  REQUIRE_ARGC("MUTEX-LOCK", 1);
  ld_mutex = eval(FIRST_ARG, env);
  ltm = get_special(ld_mutex, sym_mutex);

  if (pthread_mutex_trylock(&ltm->mutex))
    return nil;     // not successful

  return ld_mutex;  // not nil value  
}

void limo_init_threading(limo_data *env)
{
  limo_data *threading_env;
  sym_thread = make_sym("THREAD");
  sym_mutex  = make_sym("MUTEX");

  sym_mutex_attr_fast   = make_sym(":FAST");
  sym_mutex_attr_rec    = make_sym(":RECURSIVE");
  sym_mutex_attr_errchk = make_sym(":ERRORCHECK");

  threading_env = make_env(NULL);
  INS_THREADING_BUILTIN(builtin_thread_create, "THREAD-CREATE");
  INS_THREADING_BUILTIN(builtin_thread_join, "THREAD-JOIN");

  INS_THREADING_BUILTIN(builtin_mutex_create, "MUTEX-CREATE");
  INS_THREADING_BUILTIN(builtin_mutex_lock, "MUTEX-LOCK");
  INS_THREADING_BUILTIN(builtin_mutex_unlock, "MUTEX-UNLOCK");
  INS_THREADING_BUILTIN(builtin_mutex_trylock, "MUTEX-TRYLOCK");
  
  setq(env, make_sym("_THREADING"), threading_env);
}
