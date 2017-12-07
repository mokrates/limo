#include "limo.h"
#include <stdlib.h>
#include <sys/time.h>

BUILTIN(builtin_system)
{
  limo_data *command;
  command=eval(FIRST_ARG, env);
  REQUIRE_ARGC("system", 1);
  if (!command->type == limo_TYPE_STRING)
    limo_error("system takes a STRING got something different");
  return make_number_from_long_long(system(command->data.d_string));
}

BUILTIN(builtin_time)
{
  limo_data *c;
  struct timeval tv;
  c = make_cons();
  if (-1 == gettimeofday(&tv))
    limo_error("could not gettimeofday()");
  CAR(c) = make_number_from_long_long(tv.tv_sec);
  CDR(c) = make_cons();
  CAR(CDR(c)) = make_number_from_long_long(tv.tv_usec);
  return c;
}
