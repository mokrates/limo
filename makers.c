#include "limo.h"
#include <gc/gc.h>
#include <string.h>

limo_data *make_limo_data(void)
{
  limo_data *ld = (limo_data *)GC_malloc(sizeof (limo_data));
  return memset(ld, 0, sizeof (limo_data));
}

limo_data *make_nil(void)
{
  limo_data *ld = make_limo_data();
  ld->type = limo_TYPE_CONS;
  ld->data.d_cons = NULL;
  return ld;
}

limo_data *make_cons(limo_data *car, limo_data *cdr)
{
  limo_data *ld = make_nil();
  ld->data.d_cons = (limo_cons *)GC_malloc(sizeof (limo_cons));
  ld->data.d_cons->car = car;
  ld->data.d_cons->cdr = cdr;
  return ld;
}

limo_data *make_sym(char *name)
{
  limo_data *ld = make_limo_data();
  char *cp;
  ld->type=limo_TYPE_SYMBOL;
  ld->data.d_string = (char *)GC_malloc(strlen(name) +1);
  strcpy(ld->data.d_string, name);
  cp = ld->data.d_string;
  if ((*cp) != '|')
    while (*cp) {
      (*cp) = toupper(*cp);
      ++cp;
    }

  return ld;
}

limo_data *make_builtin(limo_builtin f)
{
  limo_data *ld = make_nil();
  ld->type = limo_TYPE_BUILTIN;
  ld->data.d_builtin=f;
}

limo_data *make_env(limo_data *up)
{
  limo_data *env = make_nil();
  env->type = limo_TYPE_ENV;
  env->data.d_env = make_nil();
  if (up != NULL)
    setq (env, make_sym("_UP"), up);
  return env;
}

limo_data *make_eagain(limo_data *expr, limo_data *env)
{
  limo_data *l = make_cons(env, expr);
  limo_data *ea = make_nil();
  ea->type=limo_TYPE_EAGAIN;
  ea->data.d_eagain = l;
  return ea;
}

limo_data *make_dcons(limo_data *car, limo_data *dyncdr, limo_data *env)
{
  return make_cons(car, make_eagain(dyncdr, env));
}

limo_data *make_string(char *msg)
{
  limo_data *str = make_nil();
  str->type = limo_TYPE_STRING;
  str->data.d_string = (char *)GC_malloc(strlen(msg) + 1);
  strcpy(str->data.d_string, msg);
  return str;
}
