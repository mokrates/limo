#include "limo.h"

limo_data *make_env(limo_data *up)
{
  limo_data *env = make_limo_data();
  env->type = limo_TYPE_ENV;
  env->data.d_env = make_cons(up==NULL?make_nil():up, make_dict());
  return env;
}

limo_data **var_lookup_place(limo_data *env, limo_data *name)
{
  if (!env->type == limo_TYPE_ENV)
    limo_error("var_lookup: given env is no env");

  limo_data *up = CAR(env->data.d_env);
  limo_data *dict = CDR(env->data.d_env);
  limo_data **place;

  place=dict_get_place(dict, name);
  while (*place==NULL && !is_nil(up)) {
    env=up;
    up=CAR(env->data.d_env);
    dict=CDR(env->data.d_env);
    place=dict_get_place(dict, name);
  }

  if (*place==NULL)
    throw(make_cons(make_string("Variable not bound"), name));

  return &CDR(*place);
}

limo_data *var_lookup(limo_data *env, limo_data *name)
{
  limo_data **place;

  if (limo_equals(name, sym_env))
    return env;

  place = var_lookup_place(env, name);

  if (*place)
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
  dict_put(CDR(env->data.d_env), name, value);
}
