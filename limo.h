#include "config.h"
#include <stdio.h>
#include <setjmp.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>

#include <gc/gc.h>   // boehm GC
#include <gmp.h>     // gnu multiprecision lib

#define limo_TYPE_EMPTY   0
#define limo_TYPE_SYMBOL  1
#define limo_TYPE_CONS    2
#define limo_TYPE_LAMBDA  3
#define limo_TYPE_MACRO   4
#define limo_TYPE_BUILTIN 5
#define limo_TYPE_GMPQ    6
#define limo_TYPE_FLOAT   7
#define limo_TYPE_STRING  8

#define limo_TYPE_DICT    9

#define limo_TYPE_ENV     10
#define limo_TYPE_EAGAIN  11  // eval again (for tail-opt) (cons expt env)

#define limo_TYPE_SPECIAL 12  // special: #<special: (typemarker . #<specialintern:pointer>)>
#define limo_TYPE_SPECIAL_INTERN 13  // special: e.g. #<special: (STREAM . #<specialintern:0x12345678>)>

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
    double d_float;
    struct limo_DATA *(*d_builtin)(struct limo_DATA *arglist, struct limo_DATA *env);
    struct limo_DATA *d_lambda; // lambda, macro, env
    struct limo_DICT *d_dict;
    void *d_special_intern;
#define d_env d_lambda
#define d_eagain d_lambda
#define d_special d_lambda
  } data;
  unsigned int hash;  // for symbols and strings
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

#define CAR(x) ((x)->data.d_cons->car)
#define CDR(x) ((x)->data.d_cons->cdr)

#define BUILTIN(x) limo_data *x(limo_data *arglist, limo_data *env)

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

int limo_getc(reader_stream *);
char limo_eof(reader_stream *);
reader_stream *limo_rs_from_file(FILE *, char *filename);
reader_stream *limo_rs_from_string(char *);
reader_stream *limo_rs_make_readline(void);
void limo_ungetc(char, reader_stream *);
void limo_rs_annotate(limo_data *ld, reader_stream *rs);

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
limo_data *make_eagain(limo_data *expr, limo_data *env);
limo_data *make_dcons(limo_data *car, limo_data *dyncdr, limo_data *env);
limo_data *make_string(char *);

limo_data *make_globalenv(int, char **);

limo_data *try_catch(limo_data *try, limo_data *env);
void throw(limo_data *excp);
void limo_error(char *, ...);
void print_stacktrace(limo_data *s); // prints stacktrace s
limo_data *stacktrace;
limo_data *exception;
sigjmp_buf *ljbuf;

inline int is_nil(limo_data *);
#define is_nil(x) ((x)->type == limo_TYPE_CONS && !(x)->data.d_cons)
int limo_equals(limo_data *, limo_data *);
int list_length(limo_data *);
char *limo_strdup(char *str);

int inter_symbol(limo_data *);

unsigned int hash_string(char *);

#define DICT_INIT_SIZE (1<<3)   // only powers of 2!
limo_data *make_dict(void);
limo_dict *make_dict_size(int minused);
void dict_resize(limo_data *dict);
void dict_put(limo_data *dict, limo_data *key, limo_data *value);
limo_data **dict_get_place(limo_data *dict, limo_data *key);
limo_data *dict_remove(limo_data *dict, limo_data *key);
limo_data *dict_to_list(limo_data *dict);

limo_data *var_lookup(limo_data *env, limo_data *name);
void setq(limo_data *env, limo_data *name, limo_data *value);
void setf(limo_data *env, limo_data *name, limo_data *value);
void unsetq(limo_data *env, limo_data *name);

// for arglists
#define ZEROTH_ARG (CAR(arglist))
#define FIRST_ARG (CAR(CDR(arglist)))
#define SECOND_ARG (CAR(CDR(CDR(arglist))))
#define THIRD_ARG (CAR(CDR(CDR(CDR(arglist)))))

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
BUILTIN(builtin_exit);
BUILTIN(builtin_load);
BUILTIN(builtin_loaddll);
BUILTIN(builtin_gc_enable);
BUILTIN(builtin_gc_disable);
BUILTIN(builtin_gc_collect);
BUILTIN(builtin_gc_setmax);
BUILTIN(builtin_extract_env);
BUILTIN(builtin_sleep);
BUILTIN(builtin_string_concat);
BUILTIN(builtin_make_sym);
BUILTIN(builtin_get_annotation);

limo_data *real_eval(limo_data *form, limo_data *env);
limo_data *eval(limo_data *form, limo_data *env);
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
char *repr_number(limo_data *ld);
limo_data *make_number(void);
limo_data *make_number_from_str(char *);
limo_data *make_number_from_long_long(long long i);

limo_data *make_special(limo_data *type_symbol, void *content);
void *get_special(limo_data *expr, limo_data *type_symbol);
void writer_special(limo_data *expr);
void writer_special_intern(limo_data *expr);
limo_data *get_special_type_symbol(limo_data *expr);

#include "limpy.h"
