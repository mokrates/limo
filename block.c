#include "limo.h"

BUILTIN(builtin_block)
{
  sigjmp_buf *jb;
  limo_data *special_jb;
  limo_data *res;
  limo_data *block_env;
  int marked_constant;

  sigjmp_buf *exception_buf_safe;
  limo_data *finallystack_before;
  limo_data *fs_cur;

  if (list_length(arglist) != 3)
    limo_error("(block BLOCKNAME BODY)");

  exception_buf_safe = pk_ljbuf_get();
  finallystack_before = pk_finallystack_get();

  jb = (sigjmp_buf *)GC_malloc(sizeof (sigjmp_buf));
  special_jb = make_special(sym_block, jb);
  
  env = make_env(env);

  setq(env, FIRST_ARG, special_jb);
  if (sigsetjmp(*jb, 1)) {       // if true, longjmp happened.
    res = var_lookup(env, FIRST_ARG, &marked_constant);
    pk_ljbuf_set(exception_buf_safe);
    
    // execute finallies
    while ((fs_cur = pk_finallystack_get()) != finallystack_before) {
      eval(CDR(CAR(fs_cur)), CAR(CAR(fs_cur)));  // evaluate top finally
      pk_finallystack_set(CDR(fs_cur));          // pop top finally
    }
  }
  else
    res = eval(SECOND_ARG, env);

  // this invalidates the jmpbuf. it could be taken out
  // of the BLOCK form, and return-fromed to, and that gives 
  //a segfault. hence: invalidating
  // (setq x (block foo foo)) (return-from x) ; BOOM!
  CAR(special_jb->d_special) = nil; 

  return res;
}

BUILTIN(builtin_return_from)
{
  limo_data *res;
  sigjmp_buf *jb;
  int allen = list_length(arglist);
  int marked_constant;
  
  if (allen < 2 || allen > 3)
    limo_error("(return-from BLOCKNAME [returnvalue])");

  if (allen == 3)
    res = eval(SECOND_ARG, env);
  else
    res = nil;

  jb = (sigjmp_buf *)get_special(var_lookup(env, FIRST_ARG, &marked_constant), sym_block);
  setf(env, FIRST_ARG, res);
  siglongjmp(*jb, 1);
  // no return.
  return 0;
}
