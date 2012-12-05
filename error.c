#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <stdarg.h>

#include "limo.h"

jmp_buf *ljbuf=NULL;
limo_data *exception=NULL;

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
  jmp_buf *ljstacksafe;  // here the bufs get stacked
  limo_data *res;
  
  ljstacksafe = ljbuf;
  ljbuf = (jmp_buf *)GC_malloc(sizeof (jmp_buf));
  if (setjmp(*ljbuf)) { // if true; exception was thrown.
    ljbuf = ljstacksafe;
    return NULL;
  }

  res = eval(try, env);
  ljbuf = ljstacksafe;
  return res;
}

void throw(limo_data *excp)
{
  if (!ljbuf) {
    if (excp)
      writer(excp);
    exit(1);
  }
  exception = excp;
  setq(globalenv, sym_stacktrace, stacktrace);
  longjmp(*ljbuf, 1);
}

void limo_error(char *msg, ...)
{
  va_list ap;
  char buf[257];

  va_start(ap, msg);
  vsnprintf(buf, 256, msg, ap);
  //  printf("%s\n", buf);
  throw(make_string(buf));
}
