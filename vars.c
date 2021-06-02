#include "limo.h"
#include <assert.h>

limo_data *make_env(limo_data *up)
{
  limo_data *env = make_cons(up==NULL?nil:up, make_dict());
  env->type = limo_TYPE_ENV;  
  return env;
}

/* limo_data *make_vcache(limo_data *cons) */
/* { */
/*   limo_data *res=make_limo_data(); */
/*   res->type = limo_TYPE_VCACHE; */
/*   res->d_vcache = cons; */
/*   return res; */
/* } */

limo_data *var_lookup_place(limo_data *env, limo_data *name) // returns the cons from the dict
{
  if (env->type != limo_TYPE_ENV)
    limo_error("var_lookup: given env is no env");

  limo_data *up = CAR(env);
  limo_data *dict = CDR(env);
  limo_dict_item *place;
  limo_data *cons;

  place=dict_get_place(dict, name);
  if (place->cons == NULL && !is_nil(up)) {
    cons = var_lookup_place(up, name);

    // probably useless:
    /* if (cons == NULL) */
    /*   throw(make_cons(make_string("variable not bound"), name)); */

    place->cons = cons;
    place->flags = DI_CACHE;
    dict->d_dict->used++;
    dict_check_resize(dict);
    return cons;
  }

  /* if ((*place != NULL) && (CDR(*place) -> type) == limo_TYPE_VCACHE) { */
  /*   //    printf("cached - returning: "); writer(CDR(*place)->d_vcache); printf("\n"); */
  /*   return CDR(*place) -> d_vcache; */
  /* } */

  if (place->cons==NULL)
    throw(make_cons(make_string("Variable not bound"), name));

  //  printf("found locally - returning: "); writer(*place); printf("\n");
  return place->cons;
}

limo_data *var_lookup(limo_data *env, limo_data *name, int *constant)
{
  limo_data *place;

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
  assert(place);
  CDR(place) = value;
}

void setq(limo_data *env, limo_data *name, limo_data *value)
{
  dict_put(CDR(env), name, value);
}

void setconstq(limo_data *env, limo_data *name, limo_data *value)  // TODO: doesn't freeze yet.
{
  setq(env, name, value);
}

void unsetq(limo_data *env, limo_data *name)
{
  dict_remove(CDR(env), name);
}
