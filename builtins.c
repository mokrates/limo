#ifdef __MINGW32__
#include <malloc.h>
#endif
#include "limo.h"
#include <unistd.h>
#include <assert.h>
#ifndef __MINGW32__
#include <alloca.h>
#endif

BUILTIN(builtin_quote)
{
  return CAR(CDR(arglist));
}

BUILTIN(builtin_list)
{
  return list_eval(CDR(arglist), env);
}

BUILTIN(builtin_setq)
{
  REQUIRE_ARGC("SETQ", 2);
  limo_data *name = CAR(CDR(arglist));
  limo_data *value = eval(CAR(CDR(CDR(arglist))), env);
  setq(env, name, value);
  return value;
}

BUILTIN(builtin_unsetq)
{
  REQUIRE_ARGC("UNSETQ", 1);
  limo_data *name = FIRST_ARG;
  unsetq(env, name);
  return nil;
}

BUILTIN(builtin_lambda)
{
  limo_data *lambda;
  unsigned int nparams = 0;
  limo_data *paramlist;

  REQUIRE_ARGC("LAMBDA", 2);
  lambda = make_cons(env, arglist);
  lambda->type = limo_TYPE_LAMBDA;
  env->env_flags &=~ENV_RECLAIM;

  /* writer(arglist); */

  paramlist = CAR(CDR(arglist));
  if (paramlist->type == limo_TYPE_CONS)
    for (;paramlist->type==limo_TYPE_CONS;
	 ++nparams, paramlist=CDR(paramlist))
      ;
  if (paramlist->type != limo_TYPE_CONS)
    nparams++;

  /* printf("\n*******argc: %u\n", nparams); */
  lambda->nparams = nparams;
  
  return lambda;
}

BUILTIN(builtin_macro)
{
  limo_data *macro;
  REQUIRE_ARGC("MACRO", 2);
  macro = make_cons(env, arglist);
  env->env_flags &=~ ENV_RECLAIM;
  macro->type = limo_TYPE_MACRO;  
  return macro;
}

BUILTIN(builtin_macroexpand_1)
{
  limo_data *res, *arg;

  REQUIRE_ARGC("MACROEXPAND-1", 1);
  arg = eval(FIRST_ARG, env);   // we are a function, now.
  res = real_eval(arg, env, NULL);
  if (res->type == limo_TYPE_THUNK) {
    return CDR(res);
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
  REQUIRE_ARGC("APPLY", 1);
  limo_data *f = eval(FIRST_ARG, env);
  limo_data *al;

  if (list_length(arglist) ==3)
    al= eval(SECOND_ARG,env);
  else
    al=nil;

  if (f->type == limo_TYPE_LAMBDA) {
    limo_data *res, *c = make_cons(f, al);
    res = eval_function_call(f, c, env, 0, thunk_place);
    free_limo_data(c);
    return res;
  }
  else if (f->type == limo_TYPE_BUILTINFUN) {
    int nargs=0, i;
    limo_data *cur_ld;
    limo_data **argv;
    
    // cur_ld=al;
    // while (!is_nil(cur_ld)) { cur_ld=CDR(cur_ld); ++nargs; }
    nargs = list_length(al);  // this seems to be faster than to inline
    argv  = (limo_data **)alloca(nargs * sizeof (limo_data *));
    
    for (i=0, cur_ld=al; i<nargs; cur_ld=CDR(cur_ld), ++i)
      argv[i] = CAR(cur_ld);
    
    return f->d_builtinfun(nargs, argv);
  }

  limo_error("calling APPLY on builtins is unsupported, please wrap in a lambda");
}

BUILTIN(builtin_progn)
{
  limo_data *orig_arglist = arglist;
  arglist = CDR(arglist);

  if (is_nil(arglist)) {
#if STATIC_MACROEX
    if (!(limo_register & LR_OPTDISABLE))
      orig_arglist->optimized = make_nil();
#endif // STATIC_MACROEX
    return arglist;
  }

  if (arglist->type == limo_TYPE_CONS && is_nil(CDR(arglist))) {
#if STATIC_MACROEX
    if (!(limo_register & LR_OPTDISABLE))    
      orig_arglist->optimized = CAR(arglist);
#endif
    RETURN_THUNK(CAR(arglist), env);
  }   

  while (arglist->type == limo_TYPE_CONS && !is_nil(CDR(arglist))) {
    eval(CAR(arglist), env);
    arglist = CDR(arglist);
  }
  if (arglist->type != limo_TYPE_CONS)
    limo_error("(progn ...)");

  RETURN_THUNK(CAR(arglist), env);
}

BUILTIN(builtin_if)
{
  limo_data *cond, *ldthen, *ldelse;

  REQUIRE_ARGC("IF", 3);
  cond = eval(CAR(CDR(arglist)), env);
  ldthen = CAR(CDR(CDR(arglist)));
  ldelse = CAR(CDR(CDR(CDR(arglist))));

  if (is_nil(cond))
    RETURN_THUNK(ldelse, env);
  else
    RETURN_THUNK(ldthen, env);
}

BUILTINFUN(builtin_cons)
{
  REQUIRE_ARGC_FUN("cons", 2);
  return make_cons(argv[0], argv[1]);
}

BUILTIN(builtin_dcons)
{
  REQUIRE_ARGC("dcons", 2);
  env->env_flags &=~ENV_RECLAIM;
  return make_dcons(eval(CAR(CDR(arglist)), env), CAR(CDR(CDR(arglist))), env);
}

BUILTINFUN(builtin_consp)
{
  REQUIRE_ARGC_FUN("CONSP", 1);

  if (argv[0]->type == limo_TYPE_CONS)
    return sym_true;
  else
    return nil;
}

BUILTINFUN(builtin_car)
{
  REQUIRE_ARGC_FUN("car", 1);
  limo_data *ld = argv[0];
  if (is_nil(ld) || ld->type!=limo_TYPE_CONS)
    limo_error("error: (car CONS!=nil)");
  return CAR(ld);
}

BUILTINFUN(builtin_cdr)
{
  REQUIRE_ARGC_FUN("cdr", 1);

  limo_data *ld = argv[0];
  if (is_nil(ld) || ld->type!=limo_TYPE_CONS)
    limo_error("error: (cdr CONS!=nil)");
  return CDR(ld);
}

BUILTIN(builtin_eq)
{
  REQUIRE_ARGC("eq", 2);

  if (limo_equals(eval(FIRST_ARG, env), eval(SECOND_ARG, env)))
    return sym_true;
  else
    return nil;
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

  if (cons->type != limo_TYPE_CONS)
    limo_error("ERROR: (setcar CONS value)");

  CAR(cons) = value;
  return cons;
}

BUILTIN(builtin_setcdr)
{
  REQUIRE_ARGC("setcdr", 2);

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
  REQUIRE_ARGC("TRY", 2);
  res = try_catch(FIRST_ARG, env);
  if (!res) {
    limo_data *exception = pk_exception_get();
    limo_data *rethrow   = make_nil();
    env = make_env(env);
    setq(env, sym_stacktrace, *pk_stacktrace_get());
    setq(env, make_sym("_EXCEPTION"), exception?exception:nil);
    setq(env, make_sym("_RETHROW"), rethrow);
    res = eval(SECOND_ARG, env);
    if (res == rethrow) // not any nil - THIS nil.
      throw(exception);
    else
      return res;
  }
  return res;
}

BUILTIN(builtin_finally)
{
  limo_data *res;
  limo_finally_stack_item *finallystack_before = pk_finallystack_get();
  limo_finally_stack_item *new_finallystack_item;
  
  REQUIRE_ARGC("FINALLY", 2);

  new_finallystack_item = (limo_finally_stack_item *)GC_malloc(sizeof (limo_finally_stack_item));
  new_finallystack_item->next = finallystack_before;
  new_finallystack_item->dynenv = pk_dynamic_vars_get();
  new_finallystack_item->exc_buf = pk_ljbuf_get();
  new_finallystack_item->thunk = make_thunk(SECOND_ARG, env);
  pk_finallystack_set(new_finallystack_item);
  
  res = eval(FIRST_ARG, env);  // jump-targets are responsible to execute finallies in case of jumps (exception/return-from)

  assert(pk_finallystack_get()->next == finallystack_before);
  pk_finallystack_set(finallystack_before);
  eval(SECOND_ARG, env);
  
  return res;
}

BUILTIN(builtin_throw)
{
  REQUIRE_ARGC("THROW", 1);
  throw(eval(FIRST_ARG, env));
}

BUILTINFUN(builtin_exit)
{
  if (argc>=1) {
    REQUIRE_TYPE("EXIT", argv[0], limo_TYPE_GMPQ);
    exit(GETINTFROMMPQ(argv[0]));
  }
  else
    exit(0);
}

BUILTIN(builtin_load)
{
  if (list_length(arglist) != 2)
    limo_error("(load FILENAME)");

  limo_data *filename = eval(FIRST_ARG, env);

  if (filename->type != limo_TYPE_STRING)
    limo_error("load arg must be a string");

  load_limo_file(filename->d_string, env);
  return nil;
}

BUILTIN(builtin_mod_isinline)
{
  limo_data *modname = eval(FIRST_ARG, env);
  if (modname->type != limo_TYPE_STRING)
    limo_error("mod-isinline arg must be string");

  struct INLINE_MODLIST_ITEM *item;
  for (item=inline_mod_funs; item->name != NULL; ++item)
    if (!strcmp(item->name, modname->d_string))
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
    if (!strcmp(item->name, modname->d_string)) {
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

  handle = dlopen(filename->d_string, RTLD_LAZY);

  if (!handle)
    limo_error("dll load error: %s", dlerror());
  
  limo_dll_init = dlsym(handle, initfunction->d_string);
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
    double n=mpq_get_d(*ld->d_mpq);
    GC_set_max_heap_size(1024*n);
    return nil;
  }
  else
    limo_error("(gcsetmax KiloBytes) (2)");
}

BUILTIN(builtin_opt_disable)
{
  if (2 == list_length(arglist) &&
      is_nil(eval(FIRST_ARG, env)))
    limo_register &= ~LR_OPTDISABLE;
  else
    limo_register |= LR_OPTDISABLE;

  return nil;
}

BUILTIN(builtin_env_extract)
{
  REQUIRE_ARGC("ENV-EXTRACT", 1);
  limo_data *ld = eval(FIRST_ARG, env);
  REQUIRE_TYPE("ENV-EXTRACT", ld, limo_TYPE_ENV);
  return make_cons(CAR(ld), CDR(ld));
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

BUILTIN(builtin_usleep)
{
  if (list_length(arglist)!=2)
    limo_error("(usleep SECONDS) [1]");

  limo_data *ld = eval(FIRST_ARG, env);

  if (ld->type == limo_TYPE_GMPQ)
    usleep(mpq_get_d(*ld->d_mpq));
  else
    limo_error("(usleep SECONDS) [2]");

  return nil;
}

BUILTIN(builtin_string_concat)
{
  REQUIRE_ARGC("STRING-CONCAT", 2);
  limo_data *str1 = eval(FIRST_ARG, env);
  limo_data *str2 = eval(SECOND_ARG, env);
  if (str1->type != limo_TYPE_STRING ||
      str2->type != str1->type)
    limo_error("(string-concat STR1 STR2)");

  limo_data *res = make_limo_data();
  res->type = limo_TYPE_STRING;
  res->d_string = (char *)GC_malloc_atomic(str1->hash + str2->hash + 1);
  memcpy(res->d_string, str1->d_string, str1->hash);
  memcpy(res->d_string + str1->hash, str2->d_string, str2->hash);
  res->d_string[str1->hash + str2->hash] = '\0';
  res->hash = str1->hash + str2->hash;
  return res;
}

BUILTIN(builtin_make_sym)
{
  REQUIRE_ARGC("MAKE-SYM", 1);
  limo_data *str1 = eval(FIRST_ARG, env);
  if (str1->type != limo_TYPE_STRING)
    limo_error("(make-sym STR1)");

  return make_sym(str1->d_string);
}

BUILTIN(builtin_make_sym_uninterned)
{
  REQUIRE_ARGC("MAKE-SYM-UNINTERNED", 1);
  limo_data *str1 = eval(FIRST_ARG, env);
  if (str1->type != limo_TYPE_STRING)
    limo_error("(make-sym-uninterned STR1)");

  return make_sym_uninterned(str1->d_string);  
}

BUILTINFUN(builtin_sym_no_opt)
{
  REQUIRE_ARGC_FUN("SYM-NO-OPT", 1);
  REQUIRE_TYPE("SYM-NO-OPT", argv[0], limo_TYPE_SYMBOL);
  argv[0]->flags |= SYM_NO_OPT;
  return argv[0];
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

  return reader(limo_rs_from_string(str->d_string, env));
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
      
  return make_string(ld->d_string);
}

BUILTIN(builtin_freezeq)
{
  REQUIRE_ARGC("freezeq", 1);
  return freeze_var(env, FIRST_ARG);
}

BUILTIN(builtin_address_of)
{
  REQUIRE_ARGC("ADDRESS-OF", 1);
  return make_rational_from_long_long((long long) eval(FIRST_ARG, env));
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
  limo_data *the_env;
  limo_data *name;
  limo_data *result;
  
  REQUIRE_ARGC("env-getq", 2);
  name = SECOND_ARG;
  the_env = eval(FIRST_ARG, env);
  REQUIRE_TYPE("env-getq", the_env, limo_TYPE_ENV);  
  result = var_lookup(the_env, name);
  return result;
}

BUILTIN(builtin_env_current)
{
  env->env_flags &=~ENV_RECLAIM;
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

/////// reader-macros
BUILTINFUN(builtin_reader_stream_getc)
{
  char c[2] = "\0\0";
  reader_stream *rs;

  REQUIRE_ARGC_FUN("READER-STREAM-GETC", 1);
  rs = (reader_stream *)get_special(argv[0], sym_reader_stream);
  c[0] = limo_getc(rs);
  return make_string(c);
}

BUILTINFUN(builtin_reader_stream_ungetc)
{
  reader_stream *rs;

  REQUIRE_ARGC_FUN("READER-STREAM-UNGETC", 2);
  rs = (reader_stream *)get_special(argv[0], sym_reader_stream);
  REQUIRE_TYPE("READER-STREAM-UNGETC", argv[1], limo_TYPE_STRING);
  limo_ungetc(argv[1]->d_string[0], rs);
  return nil;
}

BUILTINFUN(builtin_reader_stream_eof)
{
  reader_stream *rs;

  REQUIRE_ARGC_FUN("READER-STREAM-EOF", 1);
  rs = (reader_stream *)get_special(argv[0], sym_reader_stream);
  if (limo_eof(rs))
    return sym_true;
  else
    return nil;
}


BUILTINFUN(builtin_read)
{
  reader_stream *rs;

  REQUIRE_ARGC_FUN("READ", 1);
  rs = (reader_stream *)get_special(argv[0], sym_reader_stream);
  return reader(rs);
}
