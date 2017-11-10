#define NDEBUG 1
#define LIMO_TAILCALL_OPT 1         // 0 is bad for stack, timing gets different, 1 is 'smoother'
#define STATIC_MACROEX    1         // 1 speeds things WAAAY up
#define HAVE_DISPLACED_POINTERS 0   // 1 is bad for performance

#define READLINE_HISTORY_FILE "~/.limo_history"

#ifndef LIMO_PREFIX
#define LIMO_PREFIX "/usr/local/lib/limo/"
#endif
