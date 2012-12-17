#include "limo.h"
#include <unistd.h>

BUILTIN(builtin_quote)
{
  return CAR(CDR(arglist));
}

BUILTIN(builtin_setq)
{
  if (list_length(arglist) != 3)
    limo_error("builtin_setq: (setq name value)");

  limo_data *name = CAR(CDR(arglist));
  limo_data *value = eval(CAR(CDR(CDR(arglist))), env);
  setq(env, name, value);
  return value;
}

BUILTIN(builtin_unsetq)
{
  if (list_length(arglist) != 2)
    limo_error("(unsetq VARNAME");

  limo_data *name = FIRST_ARG;
  unsetq(env, name);
  return make_nil();
}

BUILTIN(builtin_lambda)
{
  limo_data *lambda = make_nil();
  if (list_length(arglist) != 3)
    limo_error("lambda: too few arguments");
  lambda->type = limo_TYPE_LAMBDA;
  lambda->data.d_lambda = make_cons(env, arglist);
  return lambda;
}

BUILTIN(builtin_macro)
{
  limo_data *macro = make_nil();
  if (list_length(arglist) != 3)
    limo_error("macro: too few arguments");
  macro->type = limo_TYPE_MACRO;
  macro->data.d_lambda = make_cons(env, arglist);
  return macro;
}

BUILTIN(builtin_macroexpand_1)
{
  limo_data *res;

  if (list_length(arglist) != 2 ||
      FIRST_ARG->type!=limo_TYPE_CONS) {
    limo_error("(macroexpand-1 (MACROFORM ...))");
  }

  res = real_eval(FIRST_ARG, env);
  if (res->type == limo_TYPE_EAGAIN) {
    return make_cons(CDR(res->data.d_eagain),
		     env!=CAR(res->data.d_eagain)?
		     CAR(res->data.d_eagain):make_sym(":hidden"));
  }
  else
    return make_nil();
}

BUILTIN(builtin_eval)
{
  limo_data *alt_env = env;

  if (list_length(arglist) <2 || list_length(arglist)>3)
    limo_error("(eval EXP [ENV])");

  if (list_length(arglist) == 3)
    alt_env = eval(SECOND_ARG, env);

  return make_eagain(eval(FIRST_ARG, env), alt_env);
}

BUILTIN(builtin_apply)
{
  if (list_length(arglist) < 2)
    limo_error("apply need at least one arg");

  limo_data *f = eval(FIRST_ARG, env);
  limo_data *al;
  if (list_length(arglist) ==3)
    al= eval(SECOND_ARG,env);
  else
    al=make_nil();

  return eval_function_call(f, make_cons(f,al), env, 0);
}

BUILTIN(builtin_progn)
{
  limo_data *orig_arglist = arglist;
  arglist = CDR(arglist);

  if (is_nil(arglist)) {
#if STATIC_MACROEX
    *orig_arglist = *make_nil();
#endif // STATIC_MACROEX
    return arglist;
  }

  if (arglist->type == limo_TYPE_CONS && is_nil(CDR(arglist))) {
#if STATIC_MACROEX
    *orig_arglist = *CAR(arglist);
#endif
    return make_eagain(CAR(arglist), env);
  }   

  while (arglist->type == limo_TYPE_CONS && !is_nil(CDR(arglist))) {
    eval(CAR(arglist), env);
    arglist = CDR(arglist);
  }
  if (arglist->type != limo_TYPE_CONS)
    limo_error("(progn ...)");

  return make_eagain(CAR(arglist), env);
}

BUILTIN(builtin_if)
{
  limo_data *cond, *ldthen, *ldelse;

  if (list_length(arglist) != 4)
    limo_error("builtin_if: (if cond then else)");
  
  cond = CAR(CDR(arglist));
  ldthen = CAR(CDR(CDR(arglist)));
  ldelse = CAR(CDR(CDR(CDR(arglist))));

  if (is_nil(eval(cond, env)))
    return make_eagain(ldelse, env);
  else
    return make_eagain(ldthen, env);
}

BUILTIN(builtin_list)
{
  return list_eval(CDR(arglist), env);
}

BUILTIN(builtin_cons)
{
  return make_cons(eval(CAR(CDR(arglist)),env),
		   eval(CAR(CDR(CDR(arglist))), env));
}

BUILTIN(builtin_dcons)
{
  return make_dcons(eval(CAR(CDR(arglist)), env), CAR(CDR(CDR(arglist))), env);
}

BUILTIN(builtin_consp)
{
  if (list_length(arglist) != 2)
    limo_error("ERROR: (consp EXP)");

  if (eval(FIRST_ARG, env)->type == limo_TYPE_CONS)
    return make_sym(":T");
  else
    return make_nil();
}

BUILTIN(builtin_car)
{
  limo_data *ld = eval(FIRST_ARG, env);
  if (is_nil(ld) || ld->type!=limo_TYPE_CONS)
    limo_error("error: (car CONS!=nil)");
  return CAR(ld);
}

BUILTIN(builtin_cdr)
{
  limo_data *ld = eval(FIRST_ARG, env);
  if (is_nil(ld) || ld->type!=limo_TYPE_CONS)
    limo_error("error: (cdr CONS!=nil)");
  return CDR(ld);

}

BUILTIN(builtin_eq)
{
  if (limo_equals(eval(FIRST_ARG, env), eval(SECOND_ARG, env)))
    return make_sym(":T");
    else return make_nil();
}

BUILTIN(builtin_write)
{
  limo_data *ld = eval(FIRST_ARG, env);
  writer(ld);
  printf("\n");
  return ld;
}

BUILTIN(builtin_setf)
{
  limo_data *val=eval(SECOND_ARG, env);
  setf(env, FIRST_ARG, val);
  return val;
}

BUILTIN(builtin_setcar)
{
  limo_data *cons = eval(FIRST_ARG, env);
  limo_data *value = eval(SECOND_ARG, env);

  if (cons->type != limo_TYPE_CONS)
    limo_error("ERROR: (setcar CONS value)");

  CAR(cons) = value;
  return cons;
}

BUILTIN(builtin_setcdr)
{
  limo_data *cons = eval(FIRST_ARG, env);
  limo_data *value = eval(SECOND_ARG, env);

  if (cons->type != limo_TYPE_CONS)
    limo_error("ERROR: (setcdr CONS value)");

  CDR(cons) = value;
  return cons;
}

BUILTIN(builtin_try)
{
  limo_data *res;

  if (list_length(arglist) != 3)
    limo_error("(try <TRY> <CATCH>)");
  
  res = try_catch(FIRST_ARG, make_env(env));
  if (!res) {
    env = make_env(env);
    setq(env, make_sym("_EXCEPTION"), exception?exception:make_nil());
    return make_eagain(SECOND_ARG, make_env(env));
  }
}

BUILTIN(builtin_finally)
{
  limo_data *res;

  if (list_length(arglist) != 3)
    limo_error("(finally <TRY> <FINALLY>)");
  
  res = try_catch(FIRST_ARG, make_env(env));
  eval(SECOND_ARG, env);
  if (!res)
    throw_after_finally();
  return res;
}


BUILTIN(builtin_throw)
{
  if (list_length(arglist) != 2)
    limo_error("(try <TRY> <CATCH>)");
  
  throw(eval(FIRST_ARG, env));
}

BUILTIN(builtin_exit)
{
  exit(0);
}

BUILTIN(builtin_load)
{
  if (list_length(arglist) != 2)
    limo_error("(load FILENAME)");

  limo_data *filename = eval(FIRST_ARG, env);

  if (filename->type != limo_TYPE_STRING)
    limo_error("load arg must be a string");

  load_limo_file(filename->data.d_string, env);
  return make_nil();
}

BUILTIN(builtin_loaddll)
{
  void *handle;
  void (*limo_dll_init)(limo_data *env);

  if (list_length(arglist) != 2)
    limo_error("(loaddll FILENAME)");

  limo_data *filename = eval(FIRST_ARG, env);

  if (filename->type != limo_TYPE_STRING)
    limo_error("filename must be a string");

  handle = dlopen(filename->data.d_string, RTLD_LAZY);

  if (!handle)
    limo_error("dll load error: %s", dlerror());
  
  limo_dll_init = dlsym(handle, "limo_dll_init");
  if (!limo_dll_init)
    limo_error("dll start error: %s", dlerror());
  (*limo_dll_init)(env);
  return make_nil();
}

BUILTIN(builtin_gc_disable)
{
  GC_disable();
  return make_nil();
}

BUILTIN(builtin_gc_enable)
{
  GC_enable();
  return make_nil();
}

BUILTIN(builtin_gc_collect)
{
  GC_gcollect();
  return make_nil();
}

BUILTIN(builtin_gc_setmax)
{
  if (list_length(arglist)!=2)
    limo_error("(gcsetmax KiloBytes) (1)");

  limo_data *ld = eval(FIRST_ARG, env);
  
  if (ld->type == limo_TYPE_GMPQ) {
    double n=mpq_get_d(*ld->data.d_mpq);
    GC_set_max_heap_size(1024*n);
    return make_nil();
  }
  else
    limo_error("(gcsetmax KiloBytes) (2)");
}

BUILTIN(builtin_extract_env)
{
  if (list_length(arglist)!=2)
    limo_error("(extract-env ENV) (1)");

  limo_data *ld = eval(FIRST_ARG, env);

  if (ld->type == limo_TYPE_ENV)
    return ld->data.d_env;
  else
    limo_error("(extract-env ENV) (2)");
}

BUILTIN(builtin_sleep)
{
  if (list_length(arglist)!=2)
    limo_error("(sleep SECONDS) [1]");

  limo_data *ld = eval(FIRST_ARG, env);

  if (ld->type == limo_TYPE_GMPQ)
    sleep(mpq_get_d(*ld->data.d_mpq));
  else
    limo_error("(sleep SECONDS) [2]");

  return make_nil();
}

BUILTIN(builtin_string_concat)
{
  if (list_length(arglist) != 3)
    limo_error("(string-concat STR1 STR2)");

  limo_data *str1 = eval(FIRST_ARG, env);
  limo_data *str2 = eval(SECOND_ARG, env);
  if (str1->type != limo_TYPE_STRING ||
      str2->type != str1->type)
    limo_error("(string-concat STR1 STR2)");

  limo_data *res = make_limo_data();
  res->type = limo_TYPE_STRING;
  res->data.d_string = (char *)GC_malloc(strlen(str1->data.d_string) + strlen(str2->data.d_string) + 1);
  strcpy(res->data.d_string, str1->data.d_string);
  strcat(res->data.d_string, str2->data.d_string);
  return res;
}

BUILTIN(builtin_make_sym)
{
  if (list_length(arglist) != 2)
    limo_error("(make-sym STR)");

  limo_data *str1 = eval(FIRST_ARG, env);
  if (str1->type != limo_TYPE_STRING)
    limo_error("(make-sym STR1)");

  return make_sym_uninterned(str1->data.d_string);
}

BUILTIN(builtin_get_annotation)
{
  if (list_length(arglist) != 2)
    limo_error("(get-annotation FORM)");
  
  limo_data *form = eval(FIRST_ARG, env);
  return get_annotation(form);
}

BUILTIN(builtin_setconstq)
{
  if (list_length(arglist) != 3)
    limo_error("(setconstq NAME VALUE)");

  limo_data *name = CAR(CDR(arglist));
  limo_data *value = eval(CAR(CDR(CDR(arglist))), env);
  setq(env, name, make_const(name, value));
  return value;
}

BUILTIN(builtin_read_string)
{
  limo_data *str;
  
  if (list_length(arglist) != 2)
    limo_error("(read-string STRING)");
  
  str = eval(FIRST_ARG, env);
  if (str->type != limo_TYPE_STRING)
    limo_error("given arg is no STRING");

  return reader(limo_rs_from_string(str->data.d_string));
}
