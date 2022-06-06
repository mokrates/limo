#ifndef NDEBUG
#define NDEBUG 1   # has to de DEdefined to be turned on. setting to 0 is not enough
#endif

#define LIMO_THREADING    1

#define LIMO_TAILCALL_OPT 1         // 0 is bad for stack, timing gets different, 1 is 'smoother'
#define STATIC_MACROEX    1         // 1 speeds things WAAAY up / 0 only works without static_constex
#define STATIC_CONSTEX_HARD 0
#define STATIC_CONSTEX    1
#define HAVE_DISPLACED_POINTERS 0   // 1 is bad for performance

#define READLINE_HISTORY_FILE "~/.limo_history"

#ifndef LIMO_PREFIX
#define LIMO_PREFIX "/usr/local/lib/limo/"
#endif


//////////////////////// PLATFORMS
#ifdef ANDROID
#define NO_READLINE
#define NO_EXECINFO
#undef  LIMO_THREADING
#endif
