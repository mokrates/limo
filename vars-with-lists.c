#include "limo.h"

limo_data *make_env(limo_data *up)
{
  limo_data *env = make_nil();
  env->type = limo_TYPE_ENV;
  env->data.d_env = make_nil();
  if (up != NULL)
    setq (env, make_sym("_UP"), up);
  return env;
}

limo_data **var_lookup_place(limo_data *env, limo_data *name)
{
  limo_data *up=NULL;
  limo_data *up_sym = make_sym("_UP");

  if (!env->type == limo_TYPE_ENV)
    limo_error("var_lookup: given env is no env");

  limo_data *env_list = env->data.d_env;
  
  while (!is_nil(env_list)) {
    if (limo_equals(CAR(CAR(env_list)), name))
      return &CDR(CAR(env_list));

    if (limo_equals(CAR(CAR(env_list)), up_sym)) {
      up = CDR(CAR(env_list));
    }

    env_list = CDR(env_list);
    if (is_nil(env_list) && up) {
      env_list=up->data.d_env;
      up=NULL;
    }
  }

  throw(make_cons(make_string("variable not bound"), name));
  return NULL;
}

limo_data *var_lookup(limo_data *env, limo_data *name)
{
  if (limo_equals(name, make_sym("_ENV")))
    return env;

  limo_data **place = var_lookup_place(env, name);
  if (place)
    return *place;
  else
    return NULL;
}

void setf(limo_data *env, limo_data *name, limo_data *value)
{
  limo_data **place = var_lookup_place(env, name);
  if (place) 
    *place = value;
}

void setq(limo_data *env, limo_data *name, limo_data *value)
{
  env->data.d_env = make_cons(make_cons(name, value), env->data.d_env);
}
