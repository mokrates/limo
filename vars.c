#include "limo.h"
#include <assert.h>

limo_data *make_env(limo_data *up)
{
  limo_data *env = make_cons(up, make_dict());
  env->type = limo_TYPE_ENV; 
  return env;
}

limo_data *make_lcache(limo_data *place)
{
  limo_data *res=make_limo_data();
  res->type = limo_TYPE_LCACHE;
  res->nparams = place->nparams;
  res->ups = 0;

  /* printf("creating lcache, arg nr %u\n", res->nparams); */
  
  return res;
}

limo_data *var_lookup_place_ex(limo_data *env, limo_data *name, limo_data *opt) // returns the cons from the dict
{
  limo_data *up = CAR(env);
  limo_data *dict = CDR(env);
  limo_dict_item *place;
  limo_data *cons;

  place=dict_get_place(dict, name);
  if (place->cons == NULL && !is_nil(up)) {
    cons = var_lookup_place_ex(up, name, opt);

    if (opt && opt->optimized && opt->optimized->type == limo_TYPE_LCACHE)
      ++name->optimized->ups;

    place->cons = cons;
    place->flags = DI_CACHE;
    dict->d_dict->used++;

    dict_check_resize(dict);
    return cons;
  }

  if (place->cons==NULL)
    throw(make_cons(make_string("Variable not bound"), name));

  if ((place->flags & DI_LOCAL) && opt && !(name->flags & SYM_NO_OPT))
    opt->optimized = make_lcache(place->cons);

  return place->cons;
}

limo_data *var_lookup_ex(limo_data *env, limo_data *name, limo_data *opt)
{
  limo_data *place;

  place = var_lookup_place_ex(env, name, opt);

  if (place->ld_marked_const && opt)
    opt->optimized = make_const(name, CDR(place));
  return CDR(place);
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

// tried to use LCACHES here, but it didn't do anything to performance.
void setf(limo_data *env, limo_data *name, limo_data *value)
{
  limo_data *place;

  limo_data *opt = name->optimized;
  if (opt && opt->type == limo_TYPE_LCACHE) {
    limo_data *lc_env = env;
    int i=opt->ups;

    while (i--) lc_env=CAR(lc_env);
    place = &(CDR(lc_env)->d_dict->locals_store[opt->nparams]);
  }
  else
    place = var_lookup_place_ex(env, name, name);

  CDR(place) = value;
}

void setq(limo_data *env, limo_data *name, limo_data *value)
{
  dict_put(CDR(env), name, value);
  /* printf("#### after setq ##\n"); */
  /* writer(name); printf("\n"); */
  /* writer(CDR(env)); printf("\n"); */
}

void setconstq(limo_data *env, limo_data *name, limo_data *value)  // TODO: doesn't freeze yet.
{
  setq(env, name, value);
}

void unsetq(limo_data *env, limo_data *name)
{
  dict_remove(CDR(env), name);
}
