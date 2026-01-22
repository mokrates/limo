#include <limo.h>
#include <newt.h>

#define NEWTFUN_VOID_VOID(newtfun) BUILTINFUN(builtin_ ## newtfun) { \
  newtfun(); \
  return nil; \
}

#define NEWTFUN_INT_VOID(newtfun) BUILTINFUN(builtin_ ## newtfun) { \
    return make_rational_from_long_long(newtfun());                     \
}
