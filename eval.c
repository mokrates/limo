#include <gc/gc.h>
#include "limo.h"

limo_data *eval_function_call(limo_data *f, limo_data *call, limo_data *env, int eval_args)
{
  // (#<lambda:(env . (lambda (a b) body)) args...)
  limo_data *lambda_env, *lambda_list, *params, *body, *arglist, *earglist, *param_env;
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
  if (eval_args)
    earglist = list_eval(arglist, env);
  else
    earglist = arglist;

  // associating params with names
  param_env = make_env(lambda_env);
  while (!is_nil(params) && params->type==limo_TYPE_CONS) {
    if (is_nil(earglist) || earglist->type!=limo_TYPE_CONS )
      limo_error("eval funcall: too few arguments");
    setq(param_env, CAR(params), CAR(earglist));
    params = CDR(params);
    earglist = CDR(earglist);
  }
  if (params->type==limo_TYPE_SYMBOL)
    setq(param_env, params, earglist);

  //  printf("env: "); writer(param_env); printf("\n");

  return make_eagain(body, param_env);
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
  setq(param_env, make_sym("_CALLERENV"), env);
  while (!is_nil(params) && params->type==limo_TYPE_CONS) {
    if (is_nil(arglist))
      limo_error("too few arguments");

    setq(param_env, CAR(params), CAR(arglist));
    params = CDR(params);
    arglist = CDR(arglist);
  }
  if (params->type==limo_TYPE_SYMBOL)
    setq(param_env, params, arglist);
  
  return make_eagain(eval(body, param_env), env);
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
  static limo_data *trace_sym=NULL;
  static int level=0;

  ++level;

  if (!trace_sym)
    trace_sym = make_sym("_TRACE");

  while (again) {
    if (!is_nil(var_lookup(env, trace_sym))) {
      printf("eval(level % 3i): ", level);
      writer(form);
      printf("\n");
    }

    form=real_eval(form, env);
    if (!form)
      limo_error("eval(): wtf!");

    if (form->type == limo_TYPE_EAGAIN) {
      limo_data *next_form;
      //      printf("EAGAIN:"); writer(form); printf("\n");
      again=1;
      next_form= CDR(form->data.d_eagain);
      env      = CAR(form->data.d_eagain);
      form     = next_form;
    }
    else
      again=0;
  }
  --level;
  return form;
}

limo_data *real_eval(limo_data *ld, limo_data *env)
{
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
    else
      return var_lookup(env, ld);

  default: 
    return ld;
  }
}
