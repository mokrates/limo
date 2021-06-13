#include "limo.h"

struct block_data {
  limo_finally_stack_item *finallystack_position;
  sigjmp_buf jmpbuf;
};

BUILTIN(builtin_block)
{
  sigjmp_buf *jb;
  limo_data *special_block;
  limo_data *res;
  limo_data *block_env;
  int marked_constant;
  struct block_data *bd;

  sigjmp_buf *exception_buf_safe;
  limo_finally_stack_item *finallystack_before;

  if (list_length(arglist) != 3)
    limo_error("(block BLOCKNAME BODY)");

  exception_buf_safe = pk_ljbuf_get();
  finallystack_before = pk_finallystack_get();

  bd = (struct block_data *)GC_malloc(sizeof (struct block_data));
  bd->finallystack_position = pk_finallystack_get();
  special_block = make_special(sym_block, bd);
  
  env = make_env(env);

  setq(env, FIRST_ARG, special_block);
  if (sigsetjmp(bd->jmpbuf, 1)) {       // if true, longjmp happened.
    res = var_lookup(env, FIRST_ARG, &marked_constant);
    pk_ljbuf_set(exception_buf_safe);
  }
  else
    res = eval(SECOND_ARG, env);

  // this invalidates the jmpbuf. it could be taken out
  // of the BLOCK form, and return-fromed to, and that gives 
  //a segfault. hence: invalidating
  // (setq x (block foo foo)) (return-from x) ; BOOM!
  CAR(special_block->d_special) = nil; 

  return res;
}

BUILTIN(builtin_return_from)
{
  limo_data *res;
  struct block_data *bd;
  int allen = list_length(arglist);
  int marked_constant;

  limo_finally_stack_item *fs_cur;
    
  if (allen < 2 || allen > 3)
    limo_error("(return-from BLOCKNAME [returnvalue])");

  if (allen == 3)
    res = eval(SECOND_ARG, env);
  else
    res = nil;
  
  bd = (struct block_data *)get_special(var_lookup(env, FIRST_ARG, &marked_constant), sym_block);

  // execute finallies
  while ((fs_cur = pk_finallystack_get()) != bd->finallystack_position) {
    pk_ljbuf_set(fs_cur->exc_buf);
    pk_dynamic_vars_set(fs_cur->dynenv);
    pk_finallystack_set(fs_cur->next);          // pop top finally    
    eval(CDR(fs_cur->thunk), CAR(fs_cur->thunk));  // evaluate top finally
  }
  
  setf(env, FIRST_ARG, res);  // push result
  siglongjmp(bd->jmpbuf, 1);
  // no return.
  return 0;
}
