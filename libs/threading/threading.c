#define GC_THREADS
#include <pthread.h>
#include <limo.h>

#ifdef __CYGWIN__
#define        PTHREAD_MUTEX_FAST_NP                   PTHREAD_MUTEX_NORMAL
#define        PTHREAD_MUTEX_RECURSIVE_NP              PTHREAD_MUTEX_RECURSIVE
#define        PTHREAD_MUTEX_ERRORCHECK_NP             PTHREAD_MUTEX_ERRORCHECK
#define        PTHREAD_MUTEX_TIMED_NP                  PTHREAD_MUTEX_NORMAL
#define        PTHREAD_MUTEX_ADAPTIVE_NP               PTHREAD_MUTEX_NORMAL
#endif /* __CYGWIN__ */

// had to use GC_pthread_create and GC_pthread_join to make this work.
// segfaulted with the usual functions

#define INS_THREADING_BUILTIN(f, name) setq(threading_env, make_sym(name), make_builtin(f, name))
#define INS_THREADING_BUILTINFUN(f, name) setq(threading_env, make_sym(name), make_builtinfun(f, name))
#define INS_THREADING_VAR(name, val)   setq(threading_env, make_sym(name), val)

static limo_data *sym_thread;
static limo_data *sym_mutex;
static limo_data *sym_cond;

static limo_data *sym_mutex_attr_fast;
static limo_data *sym_mutex_attr_rec;
static limo_data *sym_mutex_attr_errchk;


typedef struct LIMO_THREADING_THREAD {
  pthread_t pthread;
} limo_threading_thread;

typedef struct LIMO_THREADING_MUTEX {
  pthread_mutex_t mutex;
} limo_threading_mutex;

typedef struct LIMO_THREADING_COND {
  pthread_cond_t cond;
} limo_threading_cond;

static void *limo_threading_entry_function(void *thread_thunk)
{
  int marked_const;
  void *make_limo_data_next = NULL;
  void *make_stacktrace_free = NULL;
  void *stacktrace = nil;
  volatile limo_data *dynamic_env;
  limo_data *null_env;
  void *flmalloc_lists[MAX_FLMALLOC_LISTS];
  
  pk_limo_data_next_set(&make_limo_data_next);
  pk_stacktrace_free_set(&make_stacktrace_free);
  pk_stacktrace_set(&stacktrace);
  pk_exception_set(nil);
  pk_finallystack_set(nil);
  memset(flmalloc_lists, 0, MAX_FLMALLOC_LISTS * sizeof (void *));
  pk_flmalloc_set(flmalloc_lists);

  dynamic_env = make_env(CDR((limo_data *)thread_thunk));
  null_env = make_env(nil);
  pk_dynamic_vars_set(dynamic_env);  
  if (NULL==try_catch(make_cons(CAR((limo_data *)thread_thunk), nil), null_env)) {
    printf("\nUNHANDLED EXCEPTION CAUGHT IN THREAD\n");    
    print_stacktrace(*pk_stacktrace_get());
    writer(pk_exception_get());
    printf("\n");
  }

  GC_reachable_here(dynamic_env);
  GC_reachable_here(thread_thunk);
  GC_reachable_here(null_env);

  return NULL;
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
  if (GC_pthread_create(&ltt->pthread,
			NULL,
			limo_threading_entry_function,
			make_cons(ld_entry_function, pk_dynamic_vars_get())))
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

BUILTINFUN(builtin_thread_setcancelstate)
{
  int old_state;
  REQUIRE_ARGC_FUN("THREAD-SETCANCELSTATE", 1);
  REQUIRE_TYPE("THREAD-SETCANCELSTATE", argv[0], limo_TYPE_GMPQ);
  if (!pthread_setcancelstate(GETINTFROMMPQ(argv[0]), &old_state))
    limo_error("THREAD-SETCANCELSTATE: invalid state");
  else
    return make_rational_from_long_long(old_state);
}

BUILTINFUN(builtin_thread_setcanceltype)
{
  int old_type;
  REQUIRE_ARGC_FUN("THREAD-SETCANCELTYPE", 1);
  REQUIRE_TYPE("THREAD-SETCANCELTYPE", argv[0], limo_TYPE_GMPQ);
  if (!pthread_setcanceltype(GETINTFROMMPQ(argv[0]), &old_type))
    limo_error("THREAD-SETCANCELTYPE: invalid type");
  else
    return make_rational_from_long_long(old_type);
}

BUILTINFUN(builtin_thread_cancel)
{
  limo_threading_thread *thr;
  REQUIRE_ARGC_FUN("THREAD-CANCEL", 1);
  thr = get_special(argv[0], sym_thread);
  if (!pthread_cancel(thr->pthread))
    limo_error("THREAD-CANCEL: no such thread");
  else
    return nil;
}

BUILTINFUN(builtin_thread_testcancel)
{
  pthread_testcancel();
  return nil;
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

  REQUIRE_ARGC("MUTEX-TRYLOCK", 1);
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

  REQUIRE_ARGC("MUTEX-UNLOCK", 1);
  ld_mutex = eval(FIRST_ARG, env);
  ltm = get_special(ld_mutex, sym_mutex);

  if (pthread_mutex_unlock(&ltm->mutex))
    return nil;     // not successful

  return ld_mutex;  // not nil value  
}

BUILTIN(builtin_cond_create)
{
  limo_threading_cond *lt_cond;

  lt_cond = (limo_threading_cond *)GC_malloc(sizeof (limo_threading_cond));
  pthread_cond_init(&lt_cond->cond, NULL);

  return make_special(sym_cond, lt_cond);
}

BUILTIN(builtin_cond_signal)
{
  limo_data *ld_cond;
  limo_threading_cond *ltc;

  REQUIRE_ARGC("COND-SIGNAL", 1);
  ld_cond = eval(FIRST_ARG, env);
  ltc = get_special(ld_cond, sym_cond);

  pthread_cond_signal(&ltc->cond);
  return nil;    
}

BUILTIN(builtin_cond_broadcast)
{
  limo_data *ld_cond;
  limo_threading_cond *ltc;

  REQUIRE_ARGC("COND-BROADCAST", 1);
  ld_cond = eval(FIRST_ARG, env);
  ltc = get_special(ld_cond, sym_cond);

  pthread_cond_broadcast(&ltc->cond);
  return nil;    
}

BUILTIN(builtin_cond_wait)
{
  limo_data *ld_cond;
  limo_data *ld_mutex;
  limo_threading_cond *ltc;
  limo_threading_mutex *ltm;

  REQUIRE_ARGC("COND-WAIT", 2);
  ld_cond = eval(FIRST_ARG, env);
  ld_mutex = eval(SECOND_ARG, env);
  ltc = get_special(ld_cond, sym_cond);
  ltm = get_special(ld_mutex, sym_mutex);

  pthread_cond_wait(&ltc->cond, &ltm->mutex);
  return nil;
}

void limo_init_threading(limo_data *env)
{
  limo_data *threading_env;
  sym_thread = make_sym("_THREADING-THREAD");
  sym_mutex  = make_sym("_THREADING-MUTEX");
  sym_cond   = make_sym("_THREADING-CONDITION");

  sym_mutex_attr_fast   = make_sym(":FAST");
  sym_mutex_attr_rec    = make_sym(":RECURSIVE");
  sym_mutex_attr_errchk = make_sym(":ERRORCHECK");

  threading_env = make_env(nil);
  INS_THREADING_BUILTIN(builtin_thread_create, "THREAD-CREATE");
  INS_THREADING_BUILTIN(builtin_thread_join, "THREAD-JOIN");

  INS_THREADING_VAR("CANCEL_ENABLE", make_rational_from_long_long(PTHREAD_CANCEL_ENABLE));
  INS_THREADING_VAR("CANCEL_DISABLE", make_rational_from_long_long(PTHREAD_CANCEL_DISABLE));
  INS_THREADING_VAR("CANCEL_DEFERRED", make_rational_from_long_long(PTHREAD_CANCEL_DEFERRED));
  INS_THREADING_VAR("CANCEL_ANSYNCHRONOUS", make_rational_from_long_long(PTHREAD_CANCEL_ASYNCHRONOUS));

  INS_THREADING_BUILTINFUN(builtin_thread_setcanceltype, "THREAD-SETCANCELTYPE");
  INS_THREADING_BUILTINFUN(builtin_thread_setcancelstate, "THREAD-SETCANCELSTATE");
  INS_THREADING_BUILTINFUN(builtin_thread_cancel, "THREAD-CANCEL");
  INS_THREADING_BUILTINFUN(builtin_thread_testcancel, "THREAD-TESTCANCEL");

  INS_THREADING_BUILTIN(builtin_mutex_create, "MUTEX-CREATE");
  INS_THREADING_BUILTIN(builtin_mutex_lock, "MUTEX-LOCK");
  INS_THREADING_BUILTIN(builtin_mutex_unlock, "MUTEX-UNLOCK");
  INS_THREADING_BUILTIN(builtin_mutex_trylock, "MUTEX-TRYLOCK");

  INS_THREADING_BUILTIN(builtin_cond_create, "COND-CREATE");
  INS_THREADING_BUILTIN(builtin_cond_signal, "COND-SIGNAL");
  INS_THREADING_BUILTIN(builtin_cond_broadcast, "COND-BROADCAST");
  INS_THREADING_BUILTIN(builtin_cond_wait, "COND-WAIT");
  
  setq(env, make_sym("_THREADING"), threading_env);
}
