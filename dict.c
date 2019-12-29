// dictionaries

// in which i try to reimplement python-dicts. because i like them
// shamelessly copied this: http://www.laurentluce.com/posts/python-dictionary-implementation/

#include "limo.h"
#include <assert.h>

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

inline unsigned hash(limo_data *ld)
{
  if (ld->type == limo_TYPE_CONST)
    limo_error("this should not happen (hash())");
  
  if (ld->type == limo_TYPE_SYMBOL) {
    if (ld->hash != 0)
      return ld->hash;
    else
      return (ld->hash = hash_string(ld->data.d_string));
  }
  // don't store the hash in ld->hash because we use that for the string-length
  else if (ld->type == limo_TYPE_STRING)
    return (ld->hash = hash_string(ld->data.d_string));
  else
    throw(make_cons(make_string("data is not hashable"), ld));
}

limo_data *make_dict(void)
{
  limo_data *ld = make_limo_data();
  ld->type = limo_TYPE_DICT; 
  ld->data.d_dict = make_dict_size(1);
  return ld;
}

limo_dict *make_dict_size(int minused)
{
  int size = DICT_INIT_SIZE;
  limo_dict *d;

  d=(limo_dict *)GC_malloc(sizeof (limo_dict));

  while (size < 3*minused)
    size<<=1;

  d->store = (limo_data **)GC_malloc(size * sizeof (limo_data *));
  memset(d->store, 0, size * sizeof (limo_data *));
  d->size = size;
  d->used = 0;
  return d;
}

void dict_resize(limo_data *dict)
{
  limo_dict *olddict=dict->data.d_dict;
  limo_dict *newdict=make_dict_size(olddict->used);
  int i;

  dict->data.d_dict = newdict;
  for (i=0; i<olddict->size; ++i)
    if (olddict->store[i] != NULL)
      dict_put_cons(dict, olddict->store[i]);
}

void dict_check_resize(limo_data *dict)
{
  if (3 * (dict->data.d_dict->used) > 2*dict->data.d_dict->size)
    dict_resize(dict);
}

void dict_put_cons(limo_data *dict, limo_data *cons)
{
  limo_data **ld_place;

  if (dict->type != limo_TYPE_DICT)
    limo_error("dict_put(): didn't get a dict.");

  dict_check_resize(dict);

  ld_place = dict_get_place(dict, CAR(cons));
  if (*ld_place == NULL) {
    dict->data.d_dict->used++;
    *ld_place = cons;
  }
  else if (CDR(*ld_place) -> type == limo_TYPE_VCACHE) {
      throw(make_cons(make_string("local variable referenced before assignment"), CAR(cons)));
  }
  else   // reuse existing cons
    CDR(*ld_place) = CDR(cons);
}

void dict_put(limo_data *dict, limo_data *key, limo_data *value)
{
  dict_put_cons(dict, make_cons(key, value));
}

limo_data **dict_get_place(limo_data *dict, limo_data *key)
{
  //  if (dict->type != limo_TYPE_DICT)
  //limo_error("dict_get_place(): didn't get a dict.");
  
  int i=0;
  unsigned int h = hash(key);
  unsigned int perturb = h;
  limo_dict *d = dict->data.d_dict;
#define PERTURB_SHIFT 5;
  
  while (1) {
    i = ((i<<2) + i + perturb + 1) % d->size;   // 5i + 1 (perturb gets eventually zero)

    if (d->store[i] == NULL ||
	limo_equals(key, CAR(d->store[i])))
      return &d->store[i];   // WILL be found!
    
    perturb >>= PERTURB_SHIFT;
  }

  limo_error("dict_get_place(): this should not happen!");
}

void dict_remove(limo_data *dict, limo_data *key)
{
  limo_data **place;

  place = dict_get_place(dict, key);
  if (place !=NULL) {
    (*place) = NULL;
    dict->data.d_dict->used--;

    // dict_resize MUST be done, or else items, which should have been stored in the same bucket as *key
    // are unfindable after removal of this. SERIOUSLY hard to find bug!
    dict_resize(dict);
  }
}

limo_data *dict_to_list(limo_data *dict)
{
  limo_data *res=make_nil();
  limo_dict *d = dict->data.d_dict;
  int i;
  for (i=0; i<d->size; ++i)
    if (d->store[i] != NULL)
      res = make_cons(d->store[i], res);

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
  limo_data **res;

  if (list_length(arglist) != 3)
    limo_error("(dict-get DICT KEY)");

  dict = eval(FIRST_ARG, env);
  if (dict->type != limo_TYPE_DICT)
    limo_error("(dict-get DICT KEY)");

  key = eval(SECOND_ARG, env);
  res = dict_get_place(dict, key);
  if (*res == NULL)
    throw(make_cons(make_string("Could not find key"), key));
  return *res;
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
  return nil;
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

BUILTIN(builtin_dict_has_key)
{
  limo_data *dict;
  limo_data *key;
  limo_data **res;

  if (list_length(arglist) != 3)
    limo_error("(dict-has-key DICT KEY)");

  dict = eval(FIRST_ARG, env);
  if (dict->type != limo_TYPE_DICT)
    limo_error("(dict-has-key DICT KEY)");

  key = eval(SECOND_ARG, env);
  res = dict_get_place(dict, key);
  if (*res == NULL)
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
