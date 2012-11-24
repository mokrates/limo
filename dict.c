// dictionaries

// in which i try to reimplement python-dicts. because i like them
// shamelessly copied this: http://www.laurentluce.com/posts/python-dictionary-implementation/

#include "limo.h"

unsigned int hash_string(char *str)
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

unsigned hash(limo_data *ld)
{
  if (ld->type == limo_TYPE_SYMBOL || ld->type == limo_TYPE_STRING) {
    if (ld->hash != 0)
      return ld->hash;
    else
      return (ld->hash = hash_string(ld->data.d_string));
  }
  else
    limo_error("data is not hashable");
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

  while (size < 2*minused)
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
      dict_put(dict, CAR(olddict->store[i]), CDR(olddict->store[i]));
}

void dict_put(limo_data *dict, limo_data *key, limo_data *value)
{
  limo_data **ld_place;

  if (dict->type != limo_TYPE_DICT)
    limo_error("dict_put(): didn't get a dict.");


  if (3 * (dict->data.d_dict->used) > 2*dict->data.d_dict->size)
    dict_resize(dict);

  ld_place = dict_get_place(dict, key);
  if (*ld_place == NULL)
    dict->data.d_dict->used++;

  *ld_place = make_cons(key, value);
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

limo_data *dict_remove(limo_data *dict, limo_data *key)
{
  limo_data **place = dict_get_place(dict, key);
  (*place) = NULL;
  dict->data.d_dict->used--;
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
