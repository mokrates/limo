#include "config.h"
#include <stdio.h>
#include <setjmp.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>
#include <pthread.h>

#include <gc/gc.h>   // boehm GC

// seit vigra_c
#ifdef __cplusplus
#undef __cplusplus
#include <gmp.h>     // gnu multiprecision lib
#define __cplusplus
#else
#include <gmp.h>
#endif

#define limo_TYPE_EMPTY   0
#define limo_TYPE_SYMBOL  1
#define limo_TYPE_CONS    2
#define limo_TYPE_LAMBDA  3
#define limo_TYPE_MACRO   4
#define limo_TYPE_BUILTIN 6
#define limo_TYPE_GMPQ    7
#define limo_TYPE_FLOAT   8
#define limo_TYPE_DOUBLE  9
#define limo_TYPE_STRING  10

#define limo_TYPE_DICT    11

#define limo_TYPE_ENV     12
#define limo_TYPE_THUNK   13  // eval again (for tail-opt) (cons expt env)
#define limo_TYPE_CONST   14  // wrapper for freeze'd constants.

#define limo_TYPE_VCACHE  15

#define limo_TYPE_SPECIAL 16  // special: #<special: (typemarker . #<specialintern:pointer>)>
#define limo_TYPE_SPECIAL_INTERN 17  // special: e.g. #<special: (STREAM . #<specialintern:0x12345678>)>

typedef struct limo_ANNOTATION {
  char *filename;
  int line;
  int col;
} limo_annotation;

typedef struct limo_DATA {
  int type;
  union {
    char *d_string;  // symbol, string
    struct limo_CONS *d_cons; // cons
    mpq_t *d_mpq; // int
    float d_float;
    double d_double;
    struct limo_DATA *(*d_builtin)(struct limo_DATA *arglist, struct limo_DATA *env);
    struct limo_DATA *d_lambda; // lambda, macro, env
    struct limo_DICT *d_dict;
    void *d_special_intern;
#define d_env d_lambda
#define d_thunk d_lambda
#define d_special d_lambda
#define d_vcache d_lambda
  } data;
  union {
    unsigned int hash;  // for symbols
    unsigned int string_length;
  };
#define ld_marked_const hash
  limo_annotation *annotation;
} limo_data;

typedef struct limo_CONS {
  limo_data *car, *cdr;
} limo_cons;

typedef struct limo_DICT {
  limo_data **store;
  int size;
  int used;
} limo_dict;

extern limo_data *globalenv;
extern limo_data *interned_symbols;

extern limo_data *sym_env;
extern limo_data *sym_callerenv;
extern limo_data *sym_trace;
extern limo_data *sym_true;
extern limo_data *sym_stacktrace;
extern limo_data *sym_underscore;
extern limo_data *sym_block;
extern limo_data *nil;

extern limo_data *traceplace;

#define CAR(x) ((x)->data.d_cons->car)
#define CDR(x) ((x)->data.d_cons->cdr)
#define TSCDR(x) (thunk_safe_cdr(x))

#define BUILTIN(x) limo_data *x(limo_data *arglist, limo_data *env)

#define REQUIRE_TYPE(fun, x, T) { if (x->type != T) limo_error(fun " - Argument Error: " #T " expected."); }
#define REQUIRE_ARGC(fun, n)    { if (list_length(arglist) < (n+1)) limo_error(fun " - at least " #n " arguments expected.");}

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
  char ungetc_buf[LIMO_UNGETC_BUF];
  int ungetc_buf_pos;
  int eof; // only for readline
  char *filename;
} reader_stream;

struct INLINE_MODLIST_ITEM { char *name; void (* fun)(limo_data *env); };
extern struct INLINE_MODLIST_ITEM inline_mod_funs[];

int limo_getc(reader_stream *);
char limo_eof(reader_stream *);
reader_stream *limo_rs_from_file(FILE *, char *filename);
reader_stream *limo_rs_from_string(char *);
reader_stream *limo_rs_make_readline(void);
void limo_ungetc(char, reader_stream *);
limo_annotation *limo_rs_annotation(reader_stream *rs);

limo_annotation *make_annotation(char *filename, int line, int col);
limo_data *get_annotation(limo_data *ld); // gets annotation from ld in readable form
limo_data *annotate(limo_data *ld, limo_annotation *la); // annotates ld with la, and returns it.
limo_data *reader(reader_stream *);
void writer(limo_data *);

void load_limo_file(char *fname, limo_data *env);

limo_data *make_limo_data();
limo_data *make_nil();
limo_data *make_cons(limo_data *, limo_data *);
limo_data *make_sym(char *);
limo_data *make_sym_uninterned(char *);
typedef limo_data *(*limo_builtin)(limo_data *, limo_data *);
limo_data *make_builtin(limo_builtin);
limo_data *make_env(limo_data *up);
limo_data *make_thunk(limo_data *expr, limo_data *env);
limo_data *make_dcons(limo_data *car, limo_data *dyncdr, limo_data *env);
limo_data *make_string(char *);

limo_data *make_globalenv(int, char **);

limo_data *try_catch(limo_data *thetry, limo_data *env);
#ifndef __cplusplus
void throw(limo_data *excp);
#endif
void throw_after_finally(void);
void limo_error(char *, ...);
void print_stacktrace(limo_data *s); // prints stacktrace s

extern pthread_key_t pk_stacktrace_key;
extern pthread_key_t pk_exception_key;
extern pthread_key_t pk_ljbuf_key;

#define pk_ljbuf_set(VAL)      (pthread_setspecific(pk_ljbuf_key, (void *)(VAL)))
#define pk_ljbuf_get()         ((sigjmp_buf *)pthread_getspecific(pk_ljbuf_key))
#define pk_stacktrace_set(VAL) (pthread_setspecific(pk_stacktrace_key, (void *)(VAL)))
#define pk_stacktrace_get()    ((limo_data *)pthread_getspecific(pk_stacktrace_key))
#define pk_exception_set(VAL)  (pthread_setspecific(pk_exception_key, (void *)(VAL)))
#define pk_exception_get()     ((limo_data *)pthread_getspecific(pk_exception_key))

int is_nil(limo_data *);
// don't call is_nil with side-effects (i.e. eval() !)
#define is_nil(x) ((x)->type == limo_TYPE_CONS && !(x)->data.d_cons)
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
void dict_put_cons(limo_data *dict, limo_data *cons);
void dict_put(limo_data *dict, limo_data *key, limo_data *value);
limo_data **dict_get_place(limo_data *dict, limo_data *key);
void dict_remove(limo_data *dict, limo_data *key);
limo_data *dict_to_list(limo_data *dict);

limo_data *var_lookup(limo_data *env, limo_data *name, int *marked_const);
limo_data *var_lookup_place(limo_data *env, limo_data *name); // returns the cons from the dict
void setq(limo_data *env, limo_data *name, limo_data *value);
void setf(limo_data *env, limo_data *name, limo_data *value);
void setconstq(limo_data *env, limo_data *name, limo_data *value);
void unsetq(limo_data *env, limo_data *name);
limo_data *thunk_safe_cdr(limo_data *x);

// for arglists
#define ZEROTH_ARG (CAR(arglist))
#define FIRST_ARG (CAR(CDR(arglist)))
#define SECOND_ARG (CAR(CDR(CDR(arglist))))
#define THIRD_ARG (CAR(CDR(CDR(CDR(arglist)))))
#define FOURTH_ARG (CAR(CDR(CDR(CDR(CDR(arglist))))))

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
BUILTIN(builtin_cons);
BUILTIN(builtin_dcons);
BUILTIN(builtin_consp);
BUILTIN(builtin_car);
BUILTIN(builtin_cdr);
BUILTIN(builtin_eq);
BUILTIN(builtin_write);
BUILTIN(builtin_try);
BUILTIN(builtin_throw);
BUILTIN(builtin_finally);
BUILTIN(builtin_exit);
BUILTIN(builtin_load);
BUILTIN(builtin_mod_isinline);
BUILTIN(builtin_mod_loadinline);
BUILTIN(builtin_loaddll);
BUILTIN(builtin_gc_enable);
BUILTIN(builtin_gc_disable);
BUILTIN(builtin_gc_collect);
BUILTIN(builtin_gc_setmax);
BUILTIN(builtin_env_extract);
BUILTIN(builtin_sleep);
BUILTIN(builtin_string_concat);
BUILTIN(builtin_make_sym);
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

/////////////////////////////////
// misc
BUILTIN(builtin_system);
BUILTIN(builtin_time);

limo_data *real_eval(limo_data *form, limo_data *env);
limo_data *eval(limo_data *form, limo_data *env);
limo_data *ld_dup(limo_data *list);
limo_data *list_dup(limo_data *list);
limo_data *list_eval(limo_data *form, limo_data *env);
limo_data *eval_function_call(limo_data *f, limo_data *call, limo_data *env, int eval_args);

////////////////////////////////////
// special builtins for types
void number_builtins(limo_data *env);
void string_builtins(limo_data *env);
void file_builtins(limo_data *env);

////////////////////////////////////
// numbers
#define LIMO_MPQ(x) (*((x)->data.d_mpq))
#define GETINTFROMMPQ(mpq)     ((int)mpq_get_d(*(mpq)->data.d_mpq))
#define GETDOUBLEFROMMPQ(mpq)  (mpq_get_d(*(mpq)->data.d_mpq))
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
limo_data *get_special_type_symbol(limo_data *expr);

limo_data *freeze_var(limo_data *name, limo_data *env);
limo_data *make_const(limo_data *name, limo_data *val);
void writer_const(limo_data *c);

void segfault(void);  // segfault to generate a stacktrace. selfmade debug stopping-point

#include "limpy.h"

#ifdef LIMO_MAKE_EXECUTABLE
extern char *limo_program_cstr;
#endif
