#include <gc/gc.h>
#include "limo.h"
#include <assert.h>

limo_data *eval_function_call(limo_data *f, limo_data *call, limo_data *env, int eval_args, limo_data *thunk_place)
{
  // (#<lambda:(env . (lambda (a b) body)) args...)
  limo_data *lambda_env, *lambda_list, *params, *body, *arglist, *param_env, **locals_store, *pdict;
  int iparam = 0;
  
  lambda_env = CAR(f);                  // closure environment
  lambda_list = CDR(f);                 
  params = CAR(CDR(lambda_list));       // list of parameter names
  body = CAR(CDR(CDR(lambda_list)));     // body of lambda
  arglist = CDR(call);                    // argumentlist in current call

  param_env = make_env(lambda_env);     // environment to store the arguments
  pdict = CDR(param_env);               // dictionary to store locals
  
  CDR(param_env)->d_dict->locals_store = locals_store = (limo_data **)GC_malloc(f->nparams * sizeof (limo_data *));
  if (eval_args) {
    while (!is_nil(params) && params->type == limo_TYPE_CONS) {
      if (is_nil(arglist) || arglist->type != limo_TYPE_CONS )
	limo_error("eval funcall: too few arguments");
      
      /* setq(param_env, CAR(params), eval(CAR(arglist), env)); */
      locals_store[iparam] = make_cons(CAR(params), eval(CAR(arglist), env));
      locals_store[iparam]->nparams = iparam;
      dict_put_cons_ex(pdict, locals_store[iparam], DI_LOCAL);
      ++iparam;

      params = CDR(params);
      arglist = CDR(arglist);
    }
  }
  else { // don't eval args
    while (!is_nil(params) && params->type==limo_TYPE_CONS) {
      if (is_nil(arglist) || arglist->type!=limo_TYPE_CONS )
	limo_error("eval funcall: too few arguments");

      if (CAR(arglist)->type == limo_TYPE_CONST) {  // TODO: das hier entfernen, wenn es nciht auftritt
	setq(param_env, CAR(params), CAR(CAR(arglist)));
	printf("DEBUG: eval_function_call wurde mit einem CONST aufgerufen\n");
      }
      else {
	/* setq(param_env, CAR(params), CAR(arglist)); */
	locals_store[iparam] = make_cons(CAR(params), CAR(arglist));
	locals_store[iparam]->nparams = iparam;
	dict_put_cons_ex(pdict, locals_store[iparam], DI_LOCAL);
	++iparam;
      }

      params = CDR(params);
      arglist = CDR(arglist);
    }
  }
  if (params->type==limo_TYPE_SYMBOL) {
    if (eval_args) {
      /* setq(param_env, params, list_eval(arglist, env)); */
      locals_store[iparam] = make_cons(params, list_eval(arglist, env));
      locals_store[iparam]->nparams = iparam;
    }
    else {
      /* setq(param_env, params, arglist); */
      locals_store[iparam] = make_cons(params, arglist);
      locals_store[iparam]->nparams = iparam;
    }
    dict_put_cons_ex(pdict, locals_store[iparam], DI_LOCAL);
  }

  if (thunk_place) {          // if we have a prepared thunk_place, use it.
    CDR(thunk_place) = body;
    CAR(thunk_place) = param_env;
    return thunk_place;
  }
  else                        // else create a new one
    return make_thunk(body, param_env);
}

limo_data *eval_macro_call(limo_data *f, limo_data *call, limo_data *env)
{
  // (#<macro:(env . (macro (a b) body)) args...)
  limo_data *macro_env, *macro_list, *params, *body, *arglist, *param_env;
  macro_env = CAR(f);
  macro_list = CDR(f);
  params = CAR(CDR(macro_list));
  body = CAR(CDR(CDR(macro_list)));
  arglist = CDR(call);

  // associating params with names
  param_env = make_env(macro_env);
  //  setq(param_env, sym_callerenv, env);  // don't need that.
  while (!is_nil(params) && params->type==limo_TYPE_CONS) {
    if (is_nil(arglist))
      limo_error("too few arguments");

    setq(param_env, CAR(params), CAR(arglist));
    params = CDR(params);
    arglist = CDR(arglist);
  }
  if (params->type==limo_TYPE_SYMBOL)
    setq(param_env, params, arglist);

  limo_data *res = eval(body, param_env);

#if STATIC_MACROEX
  if (!(limo_register & LR_OPTDISABLE))
    call->optimized = res;
  //*call = *res;  // modifying the actual call, so the macro doesn't have to be evaled again.
#endif
  return make_thunk(res, env);
}

limo_data *ld_dup(limo_data *ld)
{
  limo_data *res = make_limo_data();
  *res = *ld;
  return res;
}

limo_data *list_dup(limo_data *list)
{
  limo_data *ld;  
  limo_data **el = &ld;
  while (list->type == limo_TYPE_CONS && !is_nil(list)) {
    if (CAR(list)->type == limo_TYPE_CONS)
      (*el) = make_cons(list_dup(CAR(list)), NULL);
    else
      (*el) = make_cons(ld_dup(CAR(list)), NULL);
    el = &CDR(*el);

    list=CDR(list);
  }
  if (is_nil(list))
    (*el) = make_nil();
  else
    (*el) = ld_dup(list);
  return ld;
}

limo_data *list_eval(limo_data *list, limo_data *env)
{
  limo_data *ld;
  limo_data **el = &ld;
  while (list->type == limo_TYPE_CONS && !is_nil(list)) {
    (*el) = make_cons(eval(CAR(list), env), NULL);
    el = &CDR(*el);

    list=CDR(list);
  }
  (*el) = make_nil();
  return ld;
}

limo_data *eval(limo_data *form, limo_data *env)   // tail recursion :D
{
  limo_data *tmp_stacktrace = pk_stacktrace_get();
  limo_data *stacktrace_cons;

  limo_data thunk;
  limo_cons thunk_cons;
  thunk.type = limo_TYPE_THUNK;
  thunk.d_cons = &thunk_cons;

  if (limo_register) {        // TODO: here is a check of "things". could be used to signal Ctrl-c
    if (limo_register & LR_SIGINT) {
      limo_register &= ~LR_SIGINT;
      limo_error("Keyboard Interrupt");
    }
    if (limo_register & LR_TRACE) {
      printf("eval: ");
      writer(form);
      printf("\n");
    }
  }

  stacktrace_cons = make_cons(form, tmp_stacktrace);
  pk_stacktrace_set(stacktrace_cons);
  for (;;) {
    form=real_eval(form, env, &thunk);
    assert(form);
    assert(form->type != limo_TYPE_CONST);

    if (form->type == limo_TYPE_THUNK) {
      limo_data *next_form;
      //      printf("THUNK:"); writer(form); printf("\n");
      next_form= CDR(form);
      env      = CAR(form);
      form     = next_form;
      CAR(stacktrace_cons) = form;
    }
    else {
        pk_stacktrace_set(tmp_stacktrace);
	return form;
    }
  }

}

limo_data *real_eval(limo_data *ld, limo_data *env, limo_data *thunk_place)
{
  limo_data *res;
  /* int marked_constant; */

  while (ld->optimized)
    ld = ld->optimized;

  switch (ld->type) {
  case limo_TYPE_CONS:
    if (is_nil(ld))
      return ld;
    else {
      limo_data *f = eval(CAR(ld), env);
      switch (f->type) {
      case limo_TYPE_BUILTIN:
	return f->d_builtin(ld, env, thunk_place); 

      case limo_TYPE_MACRO:
	return eval_macro_call(f, ld, env);

      case limo_TYPE_LAMBDA:
	return eval_function_call(f, ld, env, 1, thunk_place);

      default: limo_error("expected a callable. didn't get one");
      }
    }

  case limo_TYPE_STRING:
    return ld;

  case limo_TYPE_SYMBOL:
    if (ld->d_string[0] == ':')
      return ld;
    else {
      res=var_lookup_ex(env, ld, ld);
/* #if STATIC_CONSTEX_HARD */
/*       if (!(limo_register & LR_OPTDISABLE))       */
/*         if (marked_constant) */
/*           (*ld) = *res;       */
#if STATIC_CONSTEX
      if (res->type == limo_TYPE_CONST)  // this can happen to parameters of macros.
        res=CAR(res);
      //assert(res->type != limo_TYPE_CONST);
      
      /* if (!(limo_register & LR_OPTDISABLE)) */
      /*   if (marked_constant) */
      /*     ld->optimized = make_const(ld_dup(ld), res); */
          //ld->optimized = res;
#endif
      return res;
    }

  case limo_TYPE_CONST:
    return CAR(ld);

  case limo_TYPE_LCACHE:
    /* writer(CDR(env)->d_dict->locals_store[ld->nparams]); */
    /* printf("\nresolving lcache\n"); */
    return CDR(CDR(env)->d_dict->locals_store[ld->nparams]);

  default: 
    return ld;
  }
}
