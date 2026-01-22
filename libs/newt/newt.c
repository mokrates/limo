// written against libnewt-dev 0.52.24-2ubuntu2

#include "limo_newt.h"

limo_data *sym_newtComponent;

#define INS_NEWT_BUILTIN(name) setq(limo_newt_env, make_sym(#name), make_builtin(builtin_ ## name, #name))
#define INS_NEWT_BUILTINFUN(name) setq(limo_newt_env, make_sym(#name), make_builtinfun(builtin_ ## name, #name))


NEWTFUN_INT_VOID(newtInit);
NEWTFUN_INT_VOID(newtFinished);
NEWTFUN_VOID_VOID(newtCls);

BUILTINFUN(builtin_newtResizeScreen) {
  REQUIRE_ARGC_FUN("NEWTRESIZESCREEN", 1);
  newtResizeScreen(!is_nil(argv[0]));
  return nil;
}

NEWTFUN_VOID_VOID(newtWaitForKey);
NEWTFUN_VOID_VOID(newtClearKeyBuffer);

BUILTINFUN(builtin_newtDelay) {
  REQUIRE_ARGC_FUN("NEWTDELAY", 1);
  REQUIRE_TYPE("NEWTDELAY", argv[0], limo_TYPE_GMPQ);
  newtDelay(GETINTFROMMPQ(argv[0]));
  return nil;
}

NEWTFUN_VOID_VOID(newtPopWindow);
NEWTFUN_VOID_VOID(newtPopWindowNoRefresh);
NEWTFUN_VOID_VOID(newtRefresh);
NEWTFUN_VOID_VOID(newtSuspend);
NEWTFUN_INT_VOID(newtResume);
NEWTFUN_VOID_VOID(newtRedrawHelpLine);
NEWTFUN_VOID_VOID(newtPopHelpLine);
NEWTFUN_VOID_VOID(newtBell);
NEWTFUN_VOID_VOID(newtCursorOff);
NEWTFUN_VOID_VOID(newtCursorOn);
                  
void limo_init_newt(limo_data *env)
{
  limo_data *limo_newt_env;
  sym_newtComponent = make_sym("NEWT-COMPONENT");
  limo_newt_env = make_env(nil);

  INS_NEWT_BUILTINFUN(newtInit);
  INS_NEWT_BUILTINFUN(newtFinished);
  INS_NEWT_BUILTINFUN(newtCls);
  INS_NEWT_BUILTINFUN(newtWaitForKey);
  INS_NEWT_BUILTINFUN(newtClearKeyBuffer);
  INS_NEWT_BUILTINFUN(newtPopWindow);
  INS_NEWT_BUILTINFUN(newtPopWindowNoRefresh);
  INS_NEWT_BUILTINFUN(newtRefresh);
  INS_NEWT_BUILTINFUN(newtSuspend);
  INS_NEWT_BUILTINFUN(newtResume);
  INS_NEWT_BUILTINFUN(newtRedrawHelpLine);
  INS_NEWT_BUILTINFUN(newtPopHelpLine);
  INS_NEWT_BUILTINFUN(newtBell);
  INS_NEWT_BUILTINFUN(newtCursorOff);
  INS_NEWT_BUILTINFUN(newtCursorOn);

  
  // Attributes
  setq(limo_newt_env, make_sym("NEWT_FLAGS_SET"), make_rational_from_long_long(NEWT_FLAGS_SET));
  setq(limo_newt_env, make_sym("NEWT_FLAGS_RESET"), make_rational_from_long_long(NEWT_FLAGS_RESET));
  setq(limo_newt_env, make_sym("NEWT_FLAGS_TOGGLE"), make_rational_from_long_long(NEWT_FLAGS_TOGGLE));  

  setq(env, make_sym("_NEWT"), limo_newt_env);
}
