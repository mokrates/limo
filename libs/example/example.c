#include "../../limo.h"

void limo_dll_init(limo_data *env)
{
  setq(env, make_sym("EXAMPLE"), make_sym("EXAMPLE"));
}
