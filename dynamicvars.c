#include "limo.h"

BUILTIN(builtin_get_dynamic_env)
{
  return pk_dynamic_vars_get();
}

BUILTIN(builtin_with_dynamic_env)  // TODO unwind-protect for this
{
  limo_data *res, *denv, *old_denv;

  if (list_length(arglist) != 2)
    limo_error("(with-dynamic-env <SEXP>)");

  old_denv = pk_dynamic_vars_get();
  denv = make_env(old_denv);
  pk_dynamic_vars_set(denv);
  
  res = try_catch(FIRST_ARG, env);
  pk_dynamic_vars_set(old_denv);
  if (!res)
    throw_after_finally();

  GC_reachable_here(denv);
  return res;
}
