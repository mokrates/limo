// dictionaries

// in which i try to reimplement python-dicts. because i like them
// shamelessly copied this: http://www.laurentluce.com/posts/python-dictionary-implementation/

#include "limo.h"
#include <assert.h>

inline int limo_equals(limo_data *a, limo_data *b)
{
  if (a==b)
    return 1;

  if (a->type != b->type)
    return 0;

  switch (a->type) {
  case limo_TYPE_SYMBOL:
    if (a->hash == 0 || b->hash == 0)
      return !strcmp(a->d_string, b->d_string);
    else
      return a->hash == b->hash;

  case limo_TYPE_STRING:
    if (a->hash != b->hash)
      return 0;
    else
      return !strncmp(a->d_string, b->d_string, a->hash);
    
  case limo_TYPE_CONS:
    return CAR(a) == CAR(b) && CDR(a) == CDR(b);

  case limo_TYPE_GMPQ:
    return !mpq_cmp(LIMO_MPQ(a), LIMO_MPQ(b));

  case limo_TYPE_DOUBLE:
    return a->d_double == b->d_double;

  case limo_TYPE_NIL:
    return b->type == limo_TYPE_NIL;
  }
    
  return 0;
}

static inline unsigned int hash_string(char *str)
{
  int len = strlen(str);
  int hash;
  char *p = str;

  hash = *p << 7;
  while (*p != '\0') {
    hash = (1000003 * hash) ^ (*p);
    p++;
  }
  hash ^= len;
  hash ++;   // ensure not 0; (yees... may roll over, but it's less likely that len == 0)

  return hash;
}

static inline unsigned hash(limo_data *ld)
{
  assert (ld->type != limo_TYPE_CONST);
  if (ld->type == limo_TYPE_SYMBOL) {
    if (ld->hash != 0)
      return ld->hash;
    else
      return (ld->hash = hash_string(ld->d_string));
  }
  // don't store the hash in ld->hash because we use that for the string-length
  else if (ld->type == limo_TYPE_STRING)
    return hash_string(ld->d_string);
  else
    throw(make_cons(make_string("data is not hashable"), ld));
}

limo_data *make_dict(void)
{
  limo_data *ld = make_limo_data();
  ld->type = limo_TYPE_DICT; 
  ld->d_dict = make_dict_size(1);
  return ld;
}

limo_dict *make_dict_size(int minused)
{
  int size = DICT_INIT_SIZE;
  limo_dict *d;
  void **make_dict_next = pk_dict_next_get();

  /* if (!*make_dict_next) */
  /*   *make_dict_next = GC_malloc_many(sizeof (limo_dict)); */

  /* d = (limo_dict *)*make_dict_next; */
  /* *make_dict_next = GC_NEXT(*make_dict_next); */
  d = flmalloc(sizeof (limo_dict));

  while (size < 3*minused)
    size<<=1;

  d->store = (limo_dict_item *)flmalloc(size * sizeof (limo_dict_item));
  memset(d->store, 0, size * sizeof (limo_dict_item));
  d->locals_store = NULL;
  d->size = size;
  d->used = 0;
  return d;
}

inline void dict_resize(limo_data *dict)
{
  limo_data new = *dict;
  limo_dict *olddict = dict->d_dict;
  limo_dict *newdict = make_dict_size(olddict->used);
  int i;

  /* printf("resizing\n"); */
  new.d_dict = newdict;
  new.d_dict->locals_store = olddict->locals_store;
  for (i=0; i<olddict->size; ++i)
    if (olddict->store[i].cons != NULL)
      dict_put_cons_ex(&new, olddict->store[i].cons, olddict->store[i].flags);
  dict->d_dict = newdict;
}

inline void dict_check_resize(limo_data *dict)
{
  if (3 * (dict->d_dict->used) > 2*dict->d_dict->size)
    dict_resize(dict);
}

void dict_put_cons_ex(limo_data *dict, limo_data *cons, int flags)
{
  limo_dict_item *ld_place;

  /* if (dict->type != limo_TYPE_DICT) */
  /*   limo_error("dict_put(): didn't get a dict."); */

  ld_place = dict_get_place(dict, CAR(cons));
  if (ld_place->cons == NULL) {
    dict->d_dict->used++;
    ld_place->flags = flags;
    ld_place->cons = cons;

    dict_check_resize(dict);
  }
  else if (ld_place->flags & DI_CACHE) {
      throw(make_cons(make_string("local variable referenced before assignment"), CAR(cons)));
  }
  else   // reuse existing cons
    CDR(ld_place->cons) = CDR(cons);
}

void dict_put(limo_data *dict, limo_data *key, limo_data *value)
{
  dict_put_cons(dict, make_cons(key, value));
}

limo_dict_item *dict_get_place(limo_data *dict, limo_data *key)
{
  int i=0;
  unsigned int h = hash(key);
  unsigned int perturb = h;
  limo_dict *d = dict->d_dict;

  i=h & (d->size -1);
  while (1) {
    if (d->store[i].cons == NULL ||
	limo_equals(key, CAR(d->store[i].cons)))
      return &d->store[i];   // WILL be found!
    
    i = (i+1) & (d->size -1);
  }

  limo_error("dict_get_place(): this should not happen!");
}

void dict_remove(limo_data *dict, limo_data *key)
{
  limo_dict_item *place;

  place = dict_get_place(dict, key);
  if (place->cons !=NULL) {
    place->cons = NULL;
    dict->d_dict->used--;

    // dict_resize MUST be done, or else items, which should have been stored in the same bucket as *key
    // are unfindable after removal of this. SERIOUSLY hard to find bug!
    dict_resize(dict);
  }
}

limo_data *dict_to_list(limo_data *dict)
{
  limo_data *res=make_nil();
  limo_dict *d = dict->d_dict;
  int i;
  for (i=0; i<d->size; ++i)
    if (d->store[i].cons != NULL)
      res = make_cons(d->store[i].cons, res);

  return res;
}

////////// BUILTINS ////////////

BUILTIN(builtin_make_dict)
{
  return make_dict();  
}

BUILTIN(builtin_dict_get)
{
  limo_data *dict;
  limo_data *key;
  limo_data *res;

  if (list_length(arglist) != 3)
    limo_error("(dict-get DICT KEY)");

  dict = eval(FIRST_ARG, env);
  if (dict->type != limo_TYPE_DICT)
    limo_error("(dict-get DICT KEY)");

  key = eval(SECOND_ARG, env);
  res = dict_get_place(dict, key)->cons;
  if (res == NULL)
    throw(make_cons(make_string("Could not find key"), key));
  return res;
}

BUILTIN(builtin_dict_set)
{
  limo_data *dict;
  limo_data *key;
  limo_data *value;

  if (list_length(arglist) != 4)
    limo_error("(dict-set DICT KEY VALUE)");

  dict = eval(FIRST_ARG, env);
  if (dict->type != limo_TYPE_DICT)
    limo_error("(dict-set DICT KEY VALUE)");

  key = eval(SECOND_ARG, env);
  value = eval(THIRD_ARG, env);

  dict_put(dict, key, value);
  return value;
}


BUILTIN(builtin_dict_unset)
{
  limo_data *dict;
  limo_data *key;

  if (list_length(arglist) != 3)
    limo_error("(dict-unset DICT KEY)");

  dict = eval(FIRST_ARG, env);
  if (dict->type != limo_TYPE_DICT)
    limo_error("(dict-unset DICT KEY)");

  key = eval(SECOND_ARG, env);
  dict_remove(dict, key);
  return nil;
}

BUILTINFUN(builtin_dict_has_key)
{
  limo_data *dict;
  limo_data *key;
  limo_dict_item *res;

  REQUIRE_ARGC_FUN("DICT-HAS-KEY", 2);
  REQUIRE_TYPE("DICT-HAS-KEY", argv[0], limo_TYPE_DICT);
  res = dict_get_place(argv[0], argv[1]);
  if (res->cons == NULL)
    return nil;
  else
    return sym_true;
}

BUILTIN(builtin_dict_to_list)
{
  limo_data *dict;

  if (list_length(arglist) != 2)
    limo_error("(dict-to-list DICT)");

  dict = eval(FIRST_ARG, env);
  if (dict->type != limo_TYPE_DICT)
    limo_error("(dict-to-list DICT)");  

  return dict_to_list(dict);
}

BUILTIN(builtin_dictp)
{
 limo_data *dict;

  if (list_length(arglist) != 2)
    limo_error("(dictp VALUE)");

  dict = eval(FIRST_ARG, env);
  if (dict->type != limo_TYPE_DICT)
    return nil;
  else
    return sym_true;
}
