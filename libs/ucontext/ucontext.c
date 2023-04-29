#define GC_THREADS
#include <assert.h>
#include <errno.h>
#include <ucontext.h>
#include <limo.h>

#pragma GCC warning "ucontext DOESN't work! Don't know how to make this work"

#define UCONTEXT_STACK_SIZE (1<<16)
#define INS_UCONTEXT_BUILTIN(f, name) setq(ucontext_env, make_sym(name), make_builtin(f, name))

static limo_data *sym_ucontext;

struct limo_ucontext {
  char *stack;    // pointer for the garbage-collector and finalizer
  int stack_size;
  ucontext_t uctx;
  ucontext_t octx;
  limo_data *pass_value;
  limo_data *func;
  limo_data *env;
};

void limo_ucontext_entry_function(unsigned int high_half, unsigned int low_half)
{
  limo_data *ld_ev, *ld_pass_value;
  ucontext_t ctx_swap;
  int constant;

  struct limo_ucontext *lu = (void *)((unsigned long)high_half<<32 | low_half);

  memcpy(&lu->uctx, &lu->octx, sizeof(lu->uctx));
  
  ld_pass_value = lu->pass_value;
  ld_ev = make_cons(lu->func, make_cons(make_special(sym_ucontext, lu), make_cons(ld_pass_value, nil)));
  lu->pass_value = eval(ld_ev, lu->env);  // run the actual function

  // this should no be neccessary, but
  // maybe there is a strange error in glibc?
  setcontext(&lu->uctx);
}

/* void *limo_reg_gc_stk_n_entry(struct GC_stack_base *gcsb, struct limo_ucontext *lu) */
/* { */
/*   // hmmm, i cannot register the new stack. but must I? It is allocated using the GC. */
  
/*   // or like this???? */
/*   //GC_add_roots(lu->stack, lu->stack+lu->stack_size+1); */

/*   //if (GC_SUCCESS != GC_register_my_thread(gcsb)) */
/*   // limo_error("couldn't register thread"); */

/*   limo_ucontext_entry_function(lu); */

/*   //GC_remove_roots(lu->stack, lu->stack+lu->stack_size+1); */
/*   //GC_unregister_my_thread();    // i have to unregister this thread, but then the GC breaks! */
/*   return NULL; */
/* } */

BUILTIN(builtin_ucontext_makecontext)
{
  limo_data *ld_func, *ld_stack_size;
  struct limo_ucontext *lu;
  char *new_stack;
  int stack_size;

  REQUIRE_ARGC("MAKE-CONTEXT", 2);
  ld_func = eval(FIRST_ARG, env);
  REQUIRE_TYPE("MAKE-CONTEXT", ld_func, limo_TYPE_LAMBDA);
  ld_stack_size = eval(SECOND_ARG, env);
  REQUIRE_TYPE("MAKE-CONTEXT", ld_stack_size, limo_TYPE_GMPQ);
  stack_size = GETINTFROMMPQ(ld_stack_size);

  new_stack = (char *)GC_malloc(stack_size);
  lu = (struct limo_ucontext *)GC_malloc(sizeof (struct limo_ucontext));

  lu->stack = new_stack;
  lu->stack_size = stack_size-1;
  //  &lu.uctx->uc_stack.ss_sp = new_stack;
  //&lu.uctx->uc_stack.ss_size = stack_size;
  lu->func = ld_func;
  lu->env  = env;

  // creates a context to start a thread
  // This calls GC_call_with_stack_base first, so this can register the stack base
  // directly at the beginning
  // GC_call_with_stack_base is called with a function to call (limo_reg_gc_stk_n_entry)
  // and our user info, about what should be done next.
  //  makecontext(&lu.uctx, GC_call_with_stack_base, 2, limo_reg_gc_stk_n_entry, lu);

  return make_special(sym_ucontext, lu);
}

BUILTIN(builtin_ucontext_callcc)
{
  struct limo_ucontext *lu;
  ucontext_t swap_ctx, *old;
  limo_data *pass_value;
  
  REQUIRE_ARGC("UCONTEXT-CALLCC", 1);
  lu = get_special(eval(FIRST_ARG, env), sym_ucontext);

  if (list_length(arglist) == 3)
    lu->pass_value = eval(SECOND_ARG, env);
  else
    lu->pass_value = nil;

  if (getcontext(&lu->octx) == -1)
    throw(make_cons(sym_ucontext, make_string("couldn't create context")));
  if (getcontext(&lu->uctx) == -1)
    throw(make_cons(sym_ucontext, make_string("couldn't create context")));

  lu->uctx.uc_stack.ss_sp = lu->stack;
  lu->uctx.uc_stack.ss_size = lu->stack_size;

  lu->uctx.uc_link = &lu->octx;
  unsigned int high_half, low_half;
  high_half = (unsigned int)((unsigned long)lu>>32);
  low_half  = (unsigned int)((unsigned long)lu & 0xFFFFFFFF);

  makecontext(&lu->uctx, limo_ucontext_entry_function, 2, high_half, low_half);

  if (swapcontext(&lu->octx, &lu->uctx))
    limo_error(strerror(errno));
  
  pass_value = lu->pass_value;
  
  return pass_value;
}

BUILTIN(builtin_ucontext_swapcontext)
{
  struct limo_ucontext *lu;
  ucontext_t ctx_swap;
  limo_data *pass_value;

  REQUIRE_ARGC("UCONTEXT-SWAP-CONTEXT", 1);
  lu = get_special(eval(FIRST_ARG, env), sym_ucontext);

  if (list_length(arglist) == 3)
    lu->pass_value = eval(SECOND_ARG, env);
  else
    lu->pass_value = nil;

  if (getcontext(&lu->octx) == -1)
    throw(make_cons(sym_ucontext, make_string("couldn't create context")));

  if (!swapcontext(&lu->octx, &lu->uctx))
    limo_error(strerror(errno));

  memcpy(&lu->uctx, &lu->octx, sizeof(lu->uctx));

  pass_value = lu->pass_value;
  
  return pass_value;
}


void limo_init_ucontext(limo_data *env)
{
  limo_data *ucontext_env;
  sym_ucontext = make_sym("_UCONTEXT");

  ucontext_env = make_env(nil);
  INS_UCONTEXT_BUILTIN(builtin_ucontext_makecontext, "MAKE-CONTEXT");
  INS_UCONTEXT_BUILTIN(builtin_ucontext_callcc, "CALL/CC");
  INS_UCONTEXT_BUILTIN(builtin_ucontext_swapcontext, "SWAP-CONTEXT");
  
  setq(env, make_sym("_UCONTEXT"), ucontext_env);
}
