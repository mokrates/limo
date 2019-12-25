#include <gc/gc.h>
#include "limo.h"
#include <assert.h>

limo_data *eval_function_call(limo_data *f, limo_data *call, limo_data *env, int eval_args)
{
  // (#<lambda:(env . (lambda (a b) body)) args...)
  limo_data *lambda_env, *lambda_list, *params, *body, *arglist, *param_env;
  lambda_env = CAR(f->data.d_lambda);
  //  printf("lambda_env: "); writer(lambda_env); printf("\n");
  lambda_list = CDR(f->data.d_lambda);
  //  printf("lambda_list: "); writer(lambda_list); printf("\n");
  params = CAR(CDR(lambda_list));
  //  printf("params: "); writer(params); printf("\n");
  body= CAR(CDR(CDR(lambda_list)));
  //  printf("body: "); writer(body); printf("\n");
  arglist=CDR(call);
  //  printf("arglist: "); writer(arglist); printf("\n");

  /* if (eval_args) */
  /*   earglist = list_eval(arglist, env); */
  /* else */
  /*   earglist = arglist; */

  // associating params with names
  param_env = make_env(lambda_env);
  if (eval_args) {
    while (!is_nil(params) && params->type==limo_TYPE_CONS) {
      if (is_nil(arglist) || arglist->type!=limo_TYPE_CONS )
	limo_error("eval funcall: too few arguments");
      
      setq(param_env, CAR(params), eval(CAR(arglist), env));

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
      else
	setq(param_env, CAR(params), CAR(arglist));

      params = CDR(params);
      arglist = CDR(arglist);
    }
  }
  if (params->type==limo_TYPE_SYMBOL)
    if (eval_args)
      setq(param_env, params, list_eval(arglist, env));
    else
      setq(param_env, params, arglist);

  //  printf("env: "); writer(param_env); printf("\n");

  return make_thunk(body, param_env);
}

limo_data *eval_macro_call(limo_data *f, limo_data *call, limo_data *env)
{
  // (#<macro:(env . (macro (a b) body)) args...)
  limo_data *macro_env, *macro_list, *params, *body, *arglist, *earglist, *param_env;
  macro_env = CAR(f->data.d_lambda);
  macro_list = CDR(f->data.d_lambda);
  params = CAR(CDR(macro_list));
  body= CAR(CDR(CDR(macro_list)));
  arglist=CDR(call);

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

#if STATIC_MACROEX
  body = list_dup(body);
#endif

  limo_data *res = eval(body, param_env);

#if STATIC_MACROEX
  *call = *res;  // modifying the actual call, so the macro doesn't have to be evaled again.
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
  int again=1;
  static int level=0;
  int trace = !is_nil(CDR(traceplace));
  limo_data *tmp_stacktrace = pk_stacktrace_get();

  ++level;

  while (again) {
    if (trace) {
      printf("eval(level % 3i): ", level);
      writer(form);
      printf("\n");
    }

    pk_stacktrace_set(make_cons(form, tmp_stacktrace));

    form=real_eval(form, env);
    if (!form)
      limo_error("eval(): wtf!");

    assert(form->type != limo_TYPE_CONST);

    if (form->type == limo_TYPE_THUNK) {
      limo_data *next_form;
      //      printf("THUNK:"); writer(form); printf("\n");
      again=1;
      next_form= CDR(form->data.d_thunk);
      env      = CAR(form->data.d_thunk);
      form     = next_form;
    }
    else
      again=0;
  }

  pk_stacktrace_set(tmp_stacktrace);
  --level;
  return form;
}

limo_data *real_eval(limo_data *ld, limo_data *env)
{
  limo_data *res;
  int marked_constant;

  switch (ld->type) {
  case limo_TYPE_CONS:
    if (is_nil(ld))
      return ld;
    else {
      limo_data *f = eval(CAR(ld), env);
      switch (f->type) {
      case limo_TYPE_BUILTIN:
	return f->data.d_builtin(ld, env); 

      case limo_TYPE_MACRO:
	return eval_macro_call(f, ld, env);

      case limo_TYPE_LAMBDA:
	return eval_function_call(f, ld, env, 1);

      default: limo_error("expected a callable. didn't get one");
      }
    }

  case limo_TYPE_STRING:
    return ld;

  case limo_TYPE_SYMBOL:
    if (ld->data.d_string[0] == ':')
      return ld;
    else {
      res=var_lookup(env, ld, &marked_constant);
#if STATIC_CONSTEX_HARD
      if (marked_constant)
	(*ld) = *res;      
#elif STATIC_CONSTEX
      if (res->type == limo_TYPE_CONST)  // this can happen to parameters of macros.
	res=CAR(res);
	  
      if (marked_constant)
	(*ld) = *make_const(ld_dup(ld), res);
#endif
      return res;
    }

  case limo_TYPE_CONST:
    return CAR(ld);

  default: 
    return ld;
  }
}
