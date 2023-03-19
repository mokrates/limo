#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <stdarg.h>
#include <assert.h>
#include <errno.h>

#include "limo.h"

void print_stacktrace(limo_data *s)
{
  while (!is_nil(s)) {
    writer(get_annotation(CAR(s))); printf("\n  ");
    writer(CAR(s)); printf("\n");
    s = CDR(s);
  }
}

limo_data *try_catch(limo_data *try, limo_data *env)
{
  sigjmp_buf *ljstacksafe;  // here the bufs get stacked
  limo_finally_stack_item *finallystack_before;
  limo_finally_stack_item *fs_cur;
  sigjmp_buf ljbuf;
  limo_data *res;

  ljstacksafe = pk_ljbuf_get();
  finallystack_before = pk_finallystack_get();
  
  pk_ljbuf_set(&ljbuf);
  if (sigsetjmp(ljbuf, 1)) { // if true; exception was thrown.
    pk_ljbuf_set(ljstacksafe);

    // execute finallies
    while ((fs_cur = pk_finallystack_get()) != finallystack_before) {
      assert(fs_cur->exc_buf == &ljbuf);
      pk_dynamic_vars_set(fs_cur->dynenv);
      pk_finallystack_set(fs_cur->next);          // pop top finally      
      eval(CDR(fs_cur->thunk), CAR(fs_cur->thunk));  // evaluate top finally
    }
    
    return NULL;
  }

  res = eval(try, env);
  pk_ljbuf_set(ljstacksafe);
  return res;
}

void throw(limo_data *excp)
{
  if (!pk_ljbuf_get()) {
    if (excp)
      writer(excp);
    exit(1);
  }

  pk_exception_set(excp);
  //setq(globalenv, sym_stacktrace, *pk_stacktrace_get());   // TODO: no!

  //if (*pk_ljbuf_get())
  siglongjmp(*pk_ljbuf_get(), 1);
}

// TODO: remove.
void throw_after_finally(void)
{
  if (!pk_ljbuf_get()) {
    if (pk_exception_get())
      writer(pk_exception_get());
    exit(1);
  }
  siglongjmp(*pk_ljbuf_get(), 1);
}

void limo_error(char *msg, ...)
{
  va_list ap;
  char buf[257];

  va_start(ap, msg);
  vsnprintf(buf, 256, msg, ap);
  throw(make_cons(make_sym("FATAL"), make_cons(make_string(buf), nil)));
}

void limo_error_errno(limo_data *excp_type)
{
  char buf[512];
  strerror_r(errno, buf, 512);
  throw(make_cons(excp_type, make_string(buf)));
}

void segfault()
{
  *((int *)NULL) = 123;
}

#ifdef __MINGW32__
void strerror_r(int err_no, char *buf, int buflen)
{
  strncpy(buf, strerror(err_no), buflen);
}
#endif
