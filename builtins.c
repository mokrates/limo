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
  return nil;
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
  if (res->type == limo_TYPE_THUNK) {
    return make_cons(CDR(res->data.d_thunk),
		     env!=CAR(res->data.d_thunk)?
		     CAR(res->data.d_thunk):make_sym(":hidden"));
  }
  else
    return nil;
}

BUILTIN(builtin_eval)
{
  limo_data *alt_env = env;

  if (list_length(arglist) <2 || list_length(arglist)>3)
    limo_error("(eval EXP [ENV])");

  if (list_length(arglist) == 3)
    alt_env = eval(SECOND_ARG, env);

  return make_thunk(eval(FIRST_ARG, env), alt_env);
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

  if (f->type == limo_TYPE_LAMBDA)
    return eval_function_call(f, make_cons(f,al), env, 0);
  else if (f->type == limo_TYPE_BUILTIN)
    limo_error("calling APPLY with builtins is unsupported, please wrap in a lambda");
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
    return make_thunk(CAR(arglist), env);
  }   

  while (arglist->type == limo_TYPE_CONS && !is_nil(CDR(arglist))) {
    eval(CAR(arglist), env);
    arglist = CDR(arglist);
  }
  if (arglist->type != limo_TYPE_CONS)
    limo_error("(progn ...)");

  return make_thunk(CAR(arglist), env);
}

BUILTIN(builtin_if)
{
  limo_data *cond, *ldthen, *ldelse;

  if (list_length(arglist) != 4)
    limo_error("builtin_if: (if cond then else)");
  
  cond = eval(CAR(CDR(arglist)), env);
  ldthen = CAR(CDR(CDR(arglist)));
  ldelse = CAR(CDR(CDR(CDR(arglist))));

  if (is_nil(cond))
    return make_thunk(ldelse, env);
  else
    return make_thunk(ldthen, env);
}

BUILTIN(builtin_list)
{
  return list_eval(CDR(arglist), env);
}

BUILTIN(builtin_cons)
{
  REQUIRE_ARGC("cons", 2);

  return make_cons(eval(CAR(CDR(arglist)),env),
		   eval(CAR(CDR(CDR(arglist))), env));
}

BUILTIN(builtin_dcons)
{
  REQUIRE_ARGC("dcons", 2);
  return make_dcons(eval(CAR(CDR(arglist)), env), CAR(CDR(CDR(arglist))), env);
}

BUILTIN(builtin_consp)
{
  if (list_length(arglist) != 2)
    limo_error("ERROR: (consp EXP)");

  if (eval(FIRST_ARG, env)->type == limo_TYPE_CONS)
    return make_sym(":T");
  else
    return nil;
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
  REQUIRE_ARGC("cdr", 1);

  limo_data *ld = eval(FIRST_ARG, env);
  if (is_nil(ld) || ld->type!=limo_TYPE_CONS)
    limo_error("error: (cdr CONS!=nil)");
  return CDR(ld);

}

BUILTIN(builtin_eq)
{
  REQUIRE_ARGC("eq", 2);

  if (limo_equals(eval(FIRST_ARG, env), eval(SECOND_ARG, env)))
    return make_sym(":T");
    else return nil;
}

BUILTIN(builtin_write)
{
  REQUIRE_ARGC("write", 1);

  limo_data *ld = eval(FIRST_ARG, env);
  writer(ld);
  printf("\n");
  return ld;
}

BUILTIN(builtin_write_to_list)
{
  REQUIRE_ARGC("write-to-list", 1);
  limo_data *res = make_nil();
  limo_data **dest = &res;

  limo_data *ld = eval(FIRST_ARG, env);
  l_writer(&dest, ld);
  return res;
}

BUILTIN(builtin_setf)
{
  REQUIRE_ARGC("setf", 2);

  limo_data *val=eval(SECOND_ARG, env);
  setf(env, FIRST_ARG, val);
  return val;
}

BUILTIN(builtin_setcar)
{
  REQUIRE_ARGC("setcar", 2);

  limo_data *cons = eval(FIRST_ARG, env);
  limo_data *value = eval(SECOND_ARG, env);

  if (cons->type != limo_TYPE_CONS || cons->data.d_cons == NULL)
    limo_error("ERROR: (setcar CONS value)");

  CAR(cons) = value;
  return cons;
}

BUILTIN(builtin_setcdr)
{
  REQUIRE_ARGC("setcdr", 2);

  limo_data *cons = eval(FIRST_ARG, env);
  limo_data *value = eval(SECOND_ARG, env);

  if (cons->type != limo_TYPE_CONS || cons->data.d_cons == NULL)
    limo_error("ERROR: (setcdr CONS value)");

  CDR(cons) = value;
  return cons;
}

BUILTIN(builtin_try)
{
  limo_data *res;

  if (list_length(arglist) != 3)
    limo_error("(try <TRY> <CATCH>)");
  
  res = try_catch(FIRST_ARG, env);
  if (!res) {
    limo_data *exception = pk_exception_get();
    env = make_env(env);
    setq(env, make_sym("_EXCEPTION"), exception?exception:nil);
    return make_thunk(SECOND_ARG, env);
  }
}

BUILTIN(builtin_finally)
{
  limo_data *res;

  if (list_length(arglist) != 3)
    limo_error("(finally <TRY> <FINALLY>)");
  
  res = try_catch(FIRST_ARG, env);
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
  return nil;
}

BUILTIN(builtin_mod_isinline)
{
  limo_data *modname = eval(FIRST_ARG, env);
  if (modname->type != limo_TYPE_STRING)
    limo_error("mod-isinline arg must be string");

  struct INLINE_MODLIST_ITEM *item;
  for (item=inline_mod_funs; item->name != NULL; ++item)
    if (!strcmp(item->name, modname->data.d_string))
      return sym_true;

  return make_nil();
}

BUILTIN(builtin_mod_loadinline)
{
  limo_data *modname = eval(FIRST_ARG, env);
  if (modname->type != limo_TYPE_STRING)
    limo_error("mod-loadinline arg must be string");

  struct INLINE_MODLIST_ITEM *item;
  for (item=inline_mod_funs; item->name != NULL; ++item)
    if (!strcmp(item->name, modname->data.d_string)) {
      (item->fun)(env);
      return sym_true;
    }

  return make_nil();  
}

BUILTIN(builtin_loaddll)
{
  void *handle;
  void (*limo_dll_init)(limo_data *env);

  if (list_length(arglist) != 3)
    limo_error("(loaddll FILENAME INITFUNCTION)");

  limo_data *filename = eval(FIRST_ARG, env);
  limo_data *initfunction = eval(SECOND_ARG, env);

  if (filename->type != limo_TYPE_STRING)
    limo_error("filename must be a string");
  if (initfunction->type != limo_TYPE_STRING)
    limo_error("initfunction must be a string");

  handle = dlopen(filename->data.d_string, RTLD_LAZY);

  if (!handle)
    limo_error("dll load error: %s", dlerror());
  
  limo_dll_init = dlsym(handle, initfunction->data.d_string);
  if (!limo_dll_init)
    limo_error("dll start error: %s", dlerror());
  (*limo_dll_init)(env);
  return nil;
}

BUILTIN(builtin_gc_disable)
{
  GC_disable();
  return nil;
}

BUILTIN(builtin_gc_enable)
{
  GC_enable();
  return nil;
}

BUILTIN(builtin_gc_collect)
{
  GC_gcollect();
  return nil;
}

BUILTIN(builtin_gc_setmax)
{
  if (list_length(arglist)!=2)
    limo_error("(gcsetmax KiloBytes) (1)");

  limo_data *ld = eval(FIRST_ARG, env);
  
  if (ld->type == limo_TYPE_GMPQ) {
    double n=mpq_get_d(*ld->data.d_mpq);
    GC_set_max_heap_size(1024*n);
    return nil;
  }
  else
    limo_error("(gcsetmax KiloBytes) (2)");
}

BUILTIN(builtin_env_extract)
{
  REQUIRE_ARGC("ENV-EXTRACT", 1);
  limo_data *ld = eval(FIRST_ARG, env);
  REQUIRE_TYPE("ENV-EXTRACT", ld, limo_TYPE_ENV);
  return ld->data.d_env;
}

BUILTIN(builtin_envp)
{
  REQUIRE_ARGC("ENVP", 1);
  limo_data *ld;
  ld = eval(FIRST_ARG, env);
  if (ld->type == limo_TYPE_ENV)
    return sym_true;
  else
    return nil;
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

  return nil;
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
  res->data.d_string = (char *)GC_malloc(str1->hash + str2->hash + 1);
  memcpy(res->data.d_string, str1->data.d_string, str1->hash);
  memcpy(res->data.d_string + str1->hash, str2->data.d_string, str2->hash);
  res->hash = str1->hash + str2->hash;
  return res;
}

BUILTIN(builtin_make_sym)
{
  if (list_length(arglist) != 2)
    limo_error("(make-sym STR)");

  limo_data *str1 = eval(FIRST_ARG, env);
  if (str1->type != limo_TYPE_STRING)
    limo_error("(make-sym STR1)");

  return make_sym(str1->data.d_string);
}

BUILTIN(builtin_make_sym_uninterned)
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

BUILTIN(builtin_symbolp)
{
  if (list_length(arglist) != 2)
    limo_error("(symbolp VALUE)");
  
  if (eval(FIRST_ARG, env)->type == limo_TYPE_SYMBOL)
    return sym_true;
  else
    return nil;
}

BUILTIN(builtin_symbol_to_string)
{
  limo_data *ld;
  
  if (list_length(arglist) != 2)
    limo_error("(symbol-to-string SYMBOL)");
  
  ld = eval(FIRST_ARG, env);

  if (ld->type != limo_TYPE_SYMBOL)
    limo_error("(symbol-to-string SYMBOL)");
      
  return make_string(ld->data.d_string);
}

BUILTIN(builtin_freezeq)
{
  REQUIRE_ARGC("freezeq", 1);
  return freeze_var(env, FIRST_ARG);
}

BUILTIN(builtin_address_of)
{
  REQUIRE_ARGC("ADDRESS-OF", 1);
  return make_number_from_long_long((long long) eval(FIRST_ARG, env));
}

// Environment manipulation

BUILTIN(builtin_make_env)
{
  limo_data *ld;
  
  REQUIRE_ARGC("make-env", 1);
  ld = eval(FIRST_ARG, env); 
  if (ld->type != limo_TYPE_ENV && !is_nil(ld))
    limo_error("make-env argument needs to be an env");

  return make_env(ld);
}

BUILTIN(builtin_env_setq)
{
  limo_data *value;
  limo_data *name;
  limo_data *the_env;
  
  REQUIRE_ARGC("env-setq", 3);
  the_env = eval(FIRST_ARG, env);
  name = SECOND_ARG;
  value = eval(THIRD_ARG, env);
  if (the_env->type != limo_TYPE_ENV)
    limo_error("env-setq: first argument must be an env");

  setq(the_env, name, value);
  return value;
}

BUILTIN(builtin_env_setf)
{
  limo_data *value;
  limo_data *name;
  limo_data *the_env;
  
  REQUIRE_ARGC("env-setf", 3);
  the_env = eval(FIRST_ARG, env);
  name = SECOND_ARG;
  value = eval(THIRD_ARG, env);
  if (the_env->type != limo_TYPE_ENV)
    limo_error("env-setf: first argument must be an env");

  setf(the_env, name, value);
  return value;
}

BUILTIN(builtin_env_getq)
{
  int marked_constant; // we will ignore the value here
  limo_data *the_env;
  limo_data *name;
  limo_data *result;
  
  REQUIRE_ARGC("env-getq", 2);
  name = SECOND_ARG;
  the_env = eval(FIRST_ARG, env);
  result = var_lookup(the_env, name, &marked_constant);
  return result;
}

BUILTIN(builtin_env_current)
{
  return env;
}

static void finalization_proc(limo_data *obj, limo_data *thunk)
{
  // we can return to nowhere, so there is no returning a value
  eval(make_cons(CAR(thunk), make_cons(obj, nil)), CDR(thunk));
}

BUILTIN(builtin_set_finalizer)
{
  limo_data *obj, *fun, *ofun_cons;

  obj = eval(FIRST_ARG, env);
  fun = eval(SECOND_ARG, env);
  GC_register_finalizer(obj, finalization_proc, make_cons(fun, env), NULL, &ofun_cons);
  return ofun_cons?CAR(ofun_cons):nil;
}
