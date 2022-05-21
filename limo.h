#include "config.h"
#include <stdio.h>
#include <setjmp.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>
#include <pthread.h>

#include <gc.h>   // boehm GC

// seit vigra_c
#ifdef __cplusplus
#undef __cplusplus
#include <gmp.h>     // gnu multiprecision lib
#define __cplusplus
#else
#include <gmp.h>
#endif

// limo_register
extern unsigned long long limo_register;
#define LR_SIGINT         (1<<0)
#define LR_TRACE          (1<<1)
#define LR_OPTDISABLE     (1<<2)

#define limo_TYPE_NIL            0
#define limo_TYPE_SYMBOL         1
#define limo_TYPE_CONS           2

#define limo_TYPE_LAMBDA         3
#define limo_TYPE_MACRO          4
#define limo_TYPE_BUILTIN        6
#define limo_TYPE_BUILTINFUN     7

#define limo_TYPE_GMPQ           8
#define limo_TYPE_INT            9
#define limo_TYPE_DOUBLE         10
#define limo_TYPE_STRING         11

#define limo_TYPE_DICT           12
#define limo_TYPE_ENV            13
#define limo_TYPE_THUNK          14 // eval again (for tail-opt) (cons expt env)
#define limo_TYPE_CONST          15 // wrapper for freeze'd constants.

#define limo_TYPE_VCACHE         16
#define limo_TYPE_LCACHE         17

#define limo_TYPE_SPECIAL        20 // special: #<special: (typemarker . #<specialintern:pointer>)>
#define limo_TYPE_SPECIAL_INTERN 21 // special: e.g. #<special: (STREAM . #<specialintern:0x12345678>)>

typedef struct limo_ANNOTATION {
  char *filename;
  int line;
  int col;
} limo_annotation;

typedef struct limo_DATA {
  int type;
  union {
    char *d_string;  // symbol, string
    mpq_t *d_mpq; // int
    int    d_int;
    double d_double;
    struct limo_DATA *(*d_builtin)(struct limo_DATA *arglist, struct limo_DATA *env, struct limo_DATA *thunk_place);
    struct limo_DATA *(*d_builtinfun)(int argc, struct limo_DATA **argv);
    struct limo_DATA *d_special;
    struct limo_DICT *d_dict;
    void *d_special_intern;
    struct limo_DATA *car;
  };
  struct limo_DATA *cdr;
  union {
    unsigned int hash;  // for symbols
    unsigned int string_length;
    struct {
      unsigned short nparams;
      unsigned short ld_marked_const;
    };
  };
  limo_annotation *annotation;
  struct limo_DATA *optimized;
} limo_data;

#define DI_CACHE (1<<0)
#define DI_LOCAL (1<<1)
typedef struct limo_DICT_ITEM {
  int flags;
  limo_data *cons;
} limo_dict_item;

typedef struct limo_DICT {
  limo_dict_item *store;
  int size;
  int used;

  limo_data *locals_store;
} limo_dict;

typedef struct limo_FINALLY_STACK_ITEM {
  limo_data *dynenv;
  sigjmp_buf *exc_buf;
  limo_data *thunk;
  // backtrace?

  struct limo_FINALLY_STACK_ITEM *next;
} limo_finally_stack_item;

extern limo_data *globalenv;
extern limo_data *interned_symbols;

extern limo_data *sym_env;
extern limo_data *sym_callerenv;
extern limo_data *sym_trace;
extern limo_data *sym_true;
extern limo_data *sym_stacktrace;
extern limo_data *sym_underscore;
extern limo_data *sym_block;
extern limo_data *sym_reader_stream;
extern limo_data *nil;

extern limo_data *traceplace;
extern int limo_rl_inited;

#define CAR(x) ((x)->car)
#define CDR(x) ((x)->cdr)
#define TSCDR(x) (thunk_safe_cdr(x))

#define BUILTIN(x) limo_data *x(limo_data *arglist, limo_data *env, limo_data *thunk_place)
#define BUILTINFUN(x) limo_data *x(int argc, limo_data **argv)

#define REQUIRE_TYPE(fun, x, T) { if (x->type != T) limo_error(fun " - Argument Error: " #T " expected."); }
//#define REQUIRE_ARGC(fun, n)    { if (list_length(arglist) < (n+1)) limo_error(fun " - at least " #n " arguments expected.");}
#define REQUIRE_ARGC(fun, n)    do { int _ra_i; limo_data *_ra_al; for (_ra_i=0, _ra_al=arglist; !is_nil(_ra_al); _ra_al=CDR(_ra_al), ++_ra_i) \
								     ; \
    if (_ra_i < (n+1)) limo_error(fun " - at least " #n " arguments expected."); } while (0)
#define REQUIRE_ARGC_FUN(fun, n)  do { if (argc < (n)) limo_error(fun " - at least " #n " arguments expected."); } while (0)

#define LIMO_UNGETC_BUF 20
typedef struct limo_READER_STREAM {
  union {
    FILE *file;
    char *str;
    char *readline;
  } stream;
  enum { RS_FILE, RS_STR, RS_READLINE } type;
  int pos; // linepos whereever it makes sense
  int line;
  limo_data* ungetc_buf;
  int eof; // only for readline
  char *filename;
  limo_data *env; // for dispatching
} reader_stream;

struct INLINE_MODLIST_ITEM { char *name; void (* fun)(limo_data *env); };
extern struct INLINE_MODLIST_ITEM inline_mod_funs[];

int limo_getc(reader_stream *);
char limo_eof(reader_stream *);
reader_stream *limo_rs_from_file(FILE *, char *filename, limo_data *env);
reader_stream *limo_rs_from_string(char *, limo_data *env);
reader_stream *limo_rs_make_readline(limo_data *env);
void limo_ungetc(char, reader_stream *);
limo_annotation *limo_rs_annotation(reader_stream *rs);

limo_annotation *make_annotation(char *filename, int line, int col);
limo_data *get_annotation(limo_data *ld); // gets annotation from ld in readable form
limo_data *annotate(limo_data *ld, limo_annotation *la); // annotates ld with la, and returns it.
limo_data *reader(reader_stream *);
void writer(limo_data *);
void l_writer(limo_data ***dest, limo_data *);

void load_limo_file(char *fname, limo_data *env);

limo_data *make_limo_data();
limo_data *make_nil();
limo_data *make_cons(limo_data *, limo_data *);
limo_data *make_list(int start, ...);          // start is ignored
limo_data *make_sym(char *);
limo_data *make_sym_uninterned(char *);
typedef limo_data *(*limo_builtin)(limo_data *, limo_data *, limo_data *);
typedef limo_data *(*limo_builtinfun)(int, limo_data **);
limo_data *make_builtin(limo_builtin);
limo_data *make_builtinfun(limo_builtinfun);
limo_data *make_env(limo_data *up);
limo_data *make_thunk(limo_data *expr, limo_data *env);
limo_data *make_dcons(limo_data *car, limo_data *dyncdr, limo_data *env);
limo_data *make_string(char *);
limo_data *make_char(char);

limo_data *make_globalenv(int, char **);

limo_data *try_catch(limo_data *thetry, limo_data *env);
#ifndef __cplusplus
void throw(limo_data *excp)  __attribute__ ((noreturn));
#endif
void throw_after_finally(void);
void limo_error(char *, ...) __attribute__ ((noreturn));
void limo_error_errno(limo_data *excp_name) __attribute__ ((noreturn));
void print_stacktrace(limo_data *s); // prints stacktrace s

extern pthread_key_t pk_stacktrace_key;
extern pthread_key_t pk_exception_key;
extern pthread_key_t pk_ljbuf_key;
extern pthread_key_t pk_finallystack_key;
extern pthread_key_t pk_limo_data_next_key;
extern pthread_key_t pk_stacktrace_free_key;
extern pthread_key_t pk_dict_next_key;
extern pthread_key_t pk_gmpq_next_key;
extern pthread_key_t pk_dynamic_vars_key;

#define pk_ljbuf_set(VAL)      (pthread_setspecific(pk_ljbuf_key, (void *)(VAL)))
#define pk_ljbuf_get()         ((sigjmp_buf *)pthread_getspecific(pk_ljbuf_key))
#define pk_stacktrace_set(VAL) (pthread_setspecific(pk_stacktrace_key, (void *)(VAL)))
#define pk_stacktrace_get()    ((limo_data **)pthread_getspecific(pk_stacktrace_key))
#define pk_exception_set(VAL)  (pthread_setspecific(pk_exception_key, (void *)(VAL)))
#define pk_exception_get()     ((limo_data *)pthread_getspecific(pk_exception_key))
#define pk_finallystack_set(VAL)  (pthread_setspecific(pk_finallystack_key, (void *)(VAL)))
#define pk_finallystack_get()     ((limo_finally_stack_item *)pthread_getspecific(pk_finallystack_key))
#define pk_limo_data_next_set(VAL)  (pthread_setspecific(pk_limo_data_next_key, (void *)(VAL)))
#define pk_limo_data_next_get()     ((void **)pthread_getspecific(pk_limo_data_next_key))
#define pk_stacktrace_free_set(VAL)  (pthread_setspecific(pk_stacktrace_free_key, (void *)(VAL)))
#define pk_stacktrace_free_get()       ((limo_data **)pthread_getspecific(pk_stacktrace_free_key))
#define pk_dict_next_set(VAL)  (pthread_setspecific(pk_dict_next_key, (void *)(VAL)))
#define pk_dict_next_get()       ((void **)pthread_getspecific(pk_dict_next_key))
#define pk_gmpq_next_set(VAL)  (pthread_setspecific(pk_gmpq_next_key, (void *)(VAL)))
#define pk_gmpq_next_get()       ((void **)pthread_getspecific(pk_gmpq_next_key))
#define pk_dynamic_vars_set(VAL) (pthread_setspecific(pk_dynamic_vars_key, (void *)(VAL)))
#define pk_dynamic_vars_get()    ((limo_data *)pthread_getspecific(pk_dynamic_vars_key))

int is_nil(limo_data *);
// don't call is_nil with side-effects (i.e. eval() !)
#define is_nil(x) ((x)->type == limo_TYPE_NIL)
int limo_equals(limo_data *, limo_data *);
int list_length(limo_data *);
char *limo_strdup(char *str);

int inter_symbol(limo_data *);

//unsigned int hash_string(char *);

#define DICT_INIT_SIZE (1<<4)   // only powers of 2!
limo_data *make_dict(void);
limo_dict *make_dict_size(int minused);
void dict_check_resize(limo_data *dict);
void dict_resize(limo_data *dict);
void dict_put_cons_ex(limo_data *dict, limo_data *cons, int flags);
#define dict_put_cons(a, b) dict_put_cons_ex((a), (b), 0)
void dict_put(limo_data *dict, limo_data *key, limo_data *value);
limo_dict_item *dict_get_place(limo_data *dict, limo_data *key);
void dict_remove(limo_data *dict, limo_data *key);
limo_data *dict_to_list(limo_data *dict);

#define var_lookup(env, name) var_lookup_ex((env), (name), NULL)
limo_data *var_lookup_ex(limo_data *env, limo_data *name, limo_data *opt);
#define var_lookup_place(env, name) var_lookup_place_ex((env), (name), NULL)
limo_data *var_lookup_place_ex(limo_data *env, limo_data *name, limo_data *opt); // returns the cons from the dict
void setq(limo_data *env, limo_data *name, limo_data *value);
void setf(limo_data *env, limo_data *name, limo_data *value);
void setconstq(limo_data *env, limo_data *name, limo_data *value);
void unsetq(limo_data *env, limo_data *name);
limo_data *thunk_safe_cdr(limo_data *x);

// for arglists
#define ZEROTH_ARG (CAR(arglist))
#define FIRST_ARG  (CAR(CDR(arglist)))
#define SECOND_ARG (CAR(CDR(CDR(arglist))))
#define THIRD_ARG  (CAR(CDR(CDR(CDR(arglist)))))
#define FOURTH_ARG (CAR(CDR(CDR(CDR(CDR(arglist))))))
#define FIFTH_ARG  (CAR(CDR(CDR(CDR(CDR(CDR(arglist)))))))
#define SIXTH_ARG  (CAR(CDR(CDR(CDR(CDR(CDR(CDR(arglist))))))))
#define SEVENTH_ARG (CAR(CDR(CDR(CDR(CDR(CDR(CDR(CDR(arglist)))))))))
#define EIGHTH_ARG  (CAR(CDR(CDR(CDR(CDR(CDR(CDR(CDR(CDR(arglist))))))))))
#define NINTH_ARG  (CAR(CDR(CDR(CDR(CDR(CDR(CDR(CDR(CDR(CDR(arglist)))))))))))

BUILTIN(builtin_quote);
BUILTIN(builtin_setq);
BUILTIN(builtin_unsetq);
BUILTIN(builtin_setf);
BUILTIN(builtin_setcar);
BUILTIN(builtin_setcdr);
BUILTIN(builtin_lambda);
BUILTIN(builtin_macro);
BUILTIN(builtin_macroexpand_1);
BUILTIN(builtin_eval);
BUILTIN(builtin_apply);
BUILTIN(builtin_progn);
BUILTIN(builtin_if);
BUILTIN(builtin_list);
BUILTINFUN(builtin_cons);
BUILTIN(builtin_dcons);
BUILTINFUN(builtin_consp);
BUILTINFUN(builtin_car);
BUILTINFUN(builtin_cdr);
BUILTIN(builtin_eq);
BUILTIN(builtin_write);
BUILTIN(builtin_write_to_list);
BUILTIN(builtin_try);
BUILTIN(builtin_throw);
BUILTIN(builtin_finally);
BUILTINFUN(builtin_exit);
BUILTIN(builtin_load);
BUILTIN(builtin_mod_isinline);
BUILTIN(builtin_mod_loadinline);
BUILTIN(builtin_loaddll);
BUILTIN(builtin_gc_enable);
BUILTIN(builtin_gc_disable);
BUILTIN(builtin_gc_collect);
BUILTIN(builtin_gc_setmax);
BUILTIN(builtin_opt_disable);
BUILTIN(builtin_env_extract);
BUILTIN(builtin_envp);
BUILTIN(builtin_usleep);
BUILTIN(builtin_string_concat);
BUILTIN(builtin_make_sym);
BUILTIN(builtin_make_sym_uninterned);
BUILTIN(builtin_get_annotation);
BUILTIN(builtin_make_dict);
BUILTIN(builtin_dict_get);
BUILTIN(builtin_dict_set);
BUILTIN(builtin_dict_unset);
BUILTIN(builtin_dict_to_list);
BUILTIN(builtin_dict_has_key);
BUILTIN(builtin_dictp);

BUILTIN(builtin_block);
BUILTIN(builtin_return_from);

BUILTIN(builtin_read_string);

BUILTIN(builtin_symbolp);
BUILTIN(builtin_symbol_to_string);

BUILTIN(builtin_freezeq);
BUILTIN(builtin_address_of);

// Environment manipulation

BUILTIN(builtin_make_env);
BUILTIN(builtin_env_setq);
BUILTIN(builtin_env_setf);
BUILTIN(builtin_env_getq);
BUILTIN(builtin_env_current);


// support for dynamic variables
BUILTIN(builtin_get_dynamic_env);
BUILTIN(builtin_with_dynamic_env);

BUILTIN(builtin_set_finalizer);

// Reader-Macros
BUILTINFUN(builtin_reader_stream_getc);
BUILTINFUN(builtin_reader_stream_ungetc);
BUILTINFUN(builtin_reader_stream_eof);
BUILTINFUN(builtin_read);


/////////////////////////////////
// misc
BUILTIN(builtin_system);
BUILTIN(builtin_time);

limo_data *real_eval(limo_data *form, limo_data *env, limo_data *thunk_place);
limo_data *eval(limo_data *form, limo_data *env);
limo_data *ld_dup(limo_data *list);
limo_data *list_dup(limo_data *list);
limo_data *list_eval(limo_data *form, limo_data *env);
limo_data *eval_function_call(limo_data *f, limo_data *call, limo_data *env, int eval_args, limo_data *thunk_place);

////////////////////////////////////
// special builtins for types
void number_builtins(limo_data *env);
void string_builtins(limo_data *env);
void file_builtins(limo_data *env);

////////////////////////////////////
// numbers
#define LIMO_MPQ(x) (*((x)->d_mpq))
#define GETINTFROMMPQ(mpq)     ((long long)mpq_get_d(*(mpq)->d_mpq))
#define GETDOUBLEFROMMPQ(mpq)  (mpq_get_d(*(mpq)->d_mpq))
char *repr_number(limo_data *ld);
limo_data *make_number(void);
limo_data *make_number_from_str(char *);
limo_data *make_number_from_long_long(long long i);
limo_data *make_number_from_double(double d);
double make_double_from_number(limo_data *n);

limo_data *make_special(limo_data *type_symbol, void *content);
void *get_special(limo_data *expr, limo_data *type_symbol);
void writer_special(limo_data *expr);
void writer_special_intern(limo_data *expr);
void l_writer_special(limo_data ***dest, limo_data *expr);
void l_writer_special_intern(limo_data ***dest, limo_data *expr);
limo_data *get_special_type_symbol(limo_data *expr);

limo_data *freeze_var(limo_data *name, limo_data *env);
limo_data *make_const(limo_data *name, limo_data *val);
// void writer_const(limo_data *c); // TODO delete this if it compiles without it

void segfault(void);  // segfault to generate a stacktrace. selfmade debug stopping-point

#define RETURN_THUNK(_car, _cdr) if (thunk_place) { CDR(thunk_place)=_car; CAR(thunk_place)=_cdr; return thunk_place; } else return make_thunk(_car, _cdr)

#include "limpy.h"

#ifdef LIMO_MAKE_EXECUTABLE
extern char *limo_program_cstr;
#endif
