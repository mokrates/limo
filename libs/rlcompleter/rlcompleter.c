#include <readline/readline.h>
#include <limo.h>

#define INSBUILTIN(f, name) setq(env, make_sym(name), make_builtin(f))

limo_data *sym_rlcompleter;
limo_data *ld_rlcomplete_rlcompleter;

static char *rlcompleter_gen(const char *text, int state) 
{
  static limo_data *lookup_list;
  char *res;

  if (!state) {
    lookup_list = eval(make_cons(ld_rlcomplete_rlcompleter, make_cons(make_string(text), nil)), globalenv);
  }
  
  while (!is_nil(lookup_list)) {
    res = (char *)malloc(strlen(CAR(lookup_list)->d_string)+1); // yes, malloc
    strcpy(res, CAR(lookup_list)->d_string);
    lookup_list= CDR(lookup_list);
    return res;
  }
  return NULL;
}

BUILTIN(builtin_set_rlcompleter)
{
  limo_data *ldrlc, *old_ldrlc;
  REQUIRE_ARGC("RLCOMPLETER-SET", 1);

  limo_rl_inited = 1;
  if (rl_completion_entry_function == rlcompleter_gen)
    old_ldrlc = make_cons(ld_rlcomplete_rlcompleter,
			  make_string(rl_basic_word_break_characters));
  else
    old_ldrlc = make_cons(make_special(sym_rlcompleter,
				       rl_completion_entry_function),
			  make_string(rl_basic_word_break_characters));
  
  ldrlc = eval(FIRST_ARG, env);

  REQUIRE_TYPE("RL-COMPLETER", ldrlc, limo_TYPE_CONS);
  REQUIRE_TYPE("RL-COMPLETER", CDR(ldrlc), limo_TYPE_STRING);

  rl_completer_word_break_characters = CDR(ldrlc)->d_string;
  if (CAR(ldrlc)->type == limo_TYPE_SPECIAL)
    rl_completion_entry_function = get_special(CAR(ldrlc), sym_rlcompleter);
  else {
    rl_completion_entry_function = rlcompleter_gen;
    ld_rlcomplete_rlcompleter = CAR(ldrlc);
  }

  return old_ldrlc;
}

void limo_init_rlcompleter(limo_data *env)
{
  sym_rlcompleter = make_sym("RLCOMPLETER");
  INSBUILTIN(builtin_set_rlcompleter, "SET-RLCOMPLETER");
}
