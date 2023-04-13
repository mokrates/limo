#include "limo.h"

struct block_data {
  limo_finally_stack_item *finallystack_position;
  sigjmp_buf jmpbuf;
};

BUILTIN(builtin_block)
{
  limo_data *special_block;
  limo_data *res;
  limo_data *block_env;
  struct block_data *bd;

  sigjmp_buf *exception_buf_safe;

  if (list_length(arglist) != 3)
    limo_error("(block BLOCKNAME BODY)");

  exception_buf_safe = pk_ljbuf_get();

  bd = (struct block_data *)flmalloc(sizeof (struct block_data));
  bd->finallystack_position = pk_finallystack_get();
  special_block = make_special(sym_block, bd);
  
  block_env = make_env(env);
  block_env->env_flags |= ENV_RECLAIM;  

  setq(env, FIRST_ARG, special_block);
  if (sigsetjmp(bd->jmpbuf, 1))       // if true, longjmp happened.
    res = var_lookup(env, FIRST_ARG);
  else
    res = eval(SECOND_ARG, block_env);

  pk_ljbuf_set(exception_buf_safe);   // reset longjump-buffer
  flfree(bd, sizeof (struct block_data));

  // this invalidates the jmpbuf. it could be taken out
  // of the BLOCK form, and return-fromed to, and that gives 
  //a segfault. hence: invalidating
  // (setq x (block foo foo)) (return-from x) ; BOOM!
  special_block->d_special_intern = NULL;

  if (block_env->env_flags & ENV_RECLAIM) {
    flfree(block_env->cdr->d_dict->store, block_env->cdr->d_dict->size * sizeof (limo_dict_item));
    flfree(block_env->cdr->d_dict, sizeof (limo_dict));
    free_limo_data(block_env->cdr); // dict
    free_limo_data(block_env);
  }

  return res;
}

BUILTIN(builtin_return_from)
{
  limo_data *res;
  struct block_data *bd;
  int allen = list_length(arglist);

  limo_finally_stack_item *fs_cur;
    
  if (allen < 2 || allen > 3)
    limo_error("(return-from BLOCKNAME [returnvalue])");

  if (allen == 3)
    res = eval(SECOND_ARG, env);
  else
    res = nil;
  
  bd = (struct block_data *)get_special(var_lookup(env, FIRST_ARG), sym_block);
  if (!bd)
    throw(make_list(0, sym_block, make_string("block was already left"), NULL));

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
