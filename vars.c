#include "limo.h"
#include <assert.h>

limo_data *make_env(limo_data *up)
{
  limo_data *env = make_limo_data();
  env->type = limo_TYPE_ENV;
  env->data.d_env = make_cons(up==NULL?make_nil():up, make_dict());
  return env;
}

limo_data *make_vcache(limo_data *cons)
{
  limo_data *res=make_limo_data();
  res->type = limo_TYPE_VCACHE;
  res->data.d_vcache = cons;
  return res;
}

limo_data *var_lookup_place(limo_data *env, limo_data *name) // returns the cons from the dict
{
  if (env->type != limo_TYPE_ENV)
    limo_error("var_lookup: given env is no env");

  limo_data *up = CAR(env->data.d_env);
  limo_data *dict = CDR(env->data.d_env);
  limo_data **place;
  limo_data *cons;

  place=dict_get_place(dict, name);
  if (*place == NULL && !is_nil(up)) {
    cons = var_lookup_place(up, name);
    if (cons == NULL)
      throw(make_cons(make_string("variable not bound"), name));

    //setq(env, name, make_vcache(cons));

    *place = make_cons(CAR(cons), make_vcache(cons));
    dict->data.d_dict->used++;
    dict_check_resize(dict);
    // printf("adding to cache - returning: "); writer(cons); printf("\n");
    return cons;
  }

  if ((*place != NULL) && (CDR(*place) -> type) == limo_TYPE_VCACHE) {
    //    printf("cached - returning: "); writer(CDR(*place)->data.d_vcache); printf("\n");
    return CDR(*place) -> data.d_vcache;
  }

  if (*place==NULL)
    throw(make_cons(make_string("Variable not bound"), name));

  //  printf("found locally - returning: "); writer(*place); printf("\n");
  return *place;
}

limo_data *var_lookup(limo_data *env, limo_data *name, int *constant)
{
  limo_data *place;

  assert(name->type != limo_TYPE_CONST);

  place = var_lookup_place(env, name);

  if (place) {
    *constant = place->ld_marked_const;
    return CDR(place);
  }
  else
    return NULL;
}

limo_data *freeze_var(limo_data *env, limo_data *name)
{
  limo_data *place;

  if (limo_equals(name, sym_env))
    return env;

  place = var_lookup_place(env, name);

  if (place) {
    place->ld_marked_const = 1;
    return sym_true;
  }
  else
    return nil;
}

void setf(limo_data *env, limo_data *name, limo_data *value)
{
  limo_data *place = var_lookup_place(env, name);
  if (place) 
    CDR(place) = value;
}

void setq(limo_data *env, limo_data *name, limo_data *value)
{
  dict_put(CDR(env->data.d_env), name, value);
}

void setconstq(limo_data *env, limo_data *name, limo_data *value)  // TODO: doesn't freeze yet.
{
  setq(env, name, value);
}

void unsetq(limo_data *env, limo_data *name)
{
  dict_remove(CDR(env->data.d_env), name);
}
