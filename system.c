#include "limo.h"
#include <stdlib.h>

BUILTIN(builtin_system)
{
  limo_data *command;
  command=eval(FIRST_ARG, env);
  REQUIRE_ARGC("system", 1);
  if (!command->type == limo_TYPE_STRING)
    limo_error("system takes a STRING got something different");
  return make_number_from_long_long(system(command->data.d_string));
}
