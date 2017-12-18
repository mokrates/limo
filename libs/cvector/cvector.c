#include <limo.h>
#include "cvector.h"

/*
  Welche Funktionalitaet benoetige ich denn ueberhaupt?
  ctypes (und sizeof) (siehe cvector.h)
  make-cvector   (/)
  cvectorp       (/)
  cvector-set    (wird in limo implementiert)
  cvector-set/unsafe 
  cvector-ref (wird in limo implementiert)
  cvector-ref/unsafe (/)
  cvector-map-slow (to be implemented in cvector.limo)
  cvector-map
  cvector-length (/)
  cvector-type   (/)
  cvector (creating a cvector from a list -> to be implemented in cvector.limo)
*/

static limo_data *cvector_type_sym;  // special type marker for limo_data-special-type
static limo_data *limo_true;         // ":T"

static int ctype_sizeof(int type)  // TODO: auf falsche eingaben prüfen!
{
  static int sizes[] = { 0, 1, 1, 2, 2, 4, 4, 8, 8, 16, 16, sizeof (float), sizeof (double) };  // there is no type 0
  return sizes[type];
}

static void ensure_cvectorp(limo_data *cv)
{
  if (cv->type == limo_TYPE_SPECIAL &&
      limo_equals(CAR(cv->data.d_special), cvector_type_sym))
    return;
  else
    limo_error("CVECTOR expected");
}

BUILTIN(builtin_ctype_sizeof)
{
  limo_data *n;
  REQUIRE_ARGC("ctype_sizeof", 1);

  n = eval(FIRST_ARG, env);
  return make_number_from_long_long(ctype_sizeof(GETINTFROMMPQ(n)));
}

BUILTIN(builtin_make_cvector)
{
  limo_data *type, *length;
  cvector *cv;
  int t, l;
  REQUIRE_ARGC("make-cvector", 2);

  type = eval(FIRST_ARG, env);
  t = GETINTFROMMPQ(type);
  
  length = eval(SECOND_ARG, env);
  l = GETINTFROMMPQ(length);
  
  cv = (cvector *)GC_malloc(sizeof (cvector));
  cv->type = t; cv->length = l;
  cv->vector = GC_malloc(ctype_sizeof(t) * l);

  return make_special(cvector_type_sym, cv);
}

BUILTIN(builtin_cvectorp)
{
  limo_data *x;

  REQUIRE_ARGC("cvectorp", 1);
  x = eval(FIRST_ARG, env);
  
  if (x->type == limo_TYPE_SPECIAL &&
      limo_equals(CAR(x->data.d_special), cvector_type_sym))
    return limo_true;
  else
    return make_nil();
}

BUILTIN(builtin_cvector_type)
{
  limo_data *cv;
  REQUIRE_ARGC("cvector-type", 1);
  cv = eval(FIRST_ARG, env);
  ensure_cvectorp(cv);
  return make_number_from_long_long(GETCVTYPE(cv));
}

BUILTIN(builtin_cvector_length)
{
  limo_data *cv;
  REQUIRE_ARGC("cvector-length", 1);
  cv = eval(FIRST_ARG, env);
  ensure_cvectorp(cv);
  return make_number_from_long_long(GETCVLENGTH(cv));
}

BUILTIN(builtin_cvector_set_unsafe)
{
  limo_data *cv, *index, *value;
  int i;
  int iv;
  float fv;
  double dv;
  
  REQUIRE_ARGC("cvector-set/unsafe", 3);
  cv = eval(FIRST_ARG, env);
  ensure_cvectorp(cv);
  index = eval(SECOND_ARG, env);
  i = GETINTFROMMPQ(index);
  value = eval(THIRD_ARG, env);

  switch (GETCVTYPE(cv)) {
  case CTYPE_DOUBLE:
    dv = make_double_from_number(value);
    ((double *)GETCVVECTOR(cv))[i] = dv;
    return value;

  case CTYPE_FLOAT:
    fv = make_double_from_number(value);
    ((float *)GETCVVECTOR(cv))[i] = fv;
    return value;
    
  case CTYPE_SINT8:
    iv = GETINTFROMMPQ(value);
    ((signed char *)GETCVVECTOR(cv))[i] = iv;
    return value;
    
  case CTYPE_SINT16:
    iv = GETINTFROMMPQ(value);
    ((signed short *)GETCVVECTOR(cv))[i] = iv;
    return value;

  case CTYPE_SINT32:
    iv = GETINTFROMMPQ(value);
    ((signed long *)GETCVVECTOR(cv))[i] = iv;
    return value;
    
  case CTYPE_SINT64:
    iv = GETINTFROMMPQ(value);
    ((signed long long *)GETCVVECTOR(cv))[i] = iv;
    return value;

  case CTYPE_UINT8:
    iv = GETINTFROMMPQ(value);
    ((unsigned char *)GETCVVECTOR(cv))[i] = iv;
    return value;
    
  case CTYPE_UINT16:
    iv = GETINTFROMMPQ(value);
    ((unsigned short *)GETCVVECTOR(cv))[i] = iv;
    return value;

  case CTYPE_UINT32:
    iv = GETINTFROMMPQ(value);
    ((unsigned long *)GETCVVECTOR(cv))[i] = iv;
    return value;
    
  case CTYPE_UINT64:
    iv = GETINTFROMMPQ(value);
    ((unsigned long long *)GETCVVECTOR(cv))[i] = iv;
    return value;

  default:
    limo_error("unknown type in cvector");
  }
}

BUILTIN(builtin_cvector_ref_unsafe)
{
  limo_data *cv, *index;
  int i;
  REQUIRE_ARGC("cvector-ref/unsafe", 2);
  cv = eval(FIRST_ARG, env);
  ensure_cvectorp(cv);
  index = eval(SECOND_ARG, env);
  i = GETINTFROMMPQ(index);

  switch (GETCVTYPE(cv)) {
  case CTYPE_DOUBLE:
    return make_number_from_double(((double *)GETCVVECTOR(cv))[i]);

  case CTYPE_FLOAT:
    return make_number_from_double(((float *)GETCVVECTOR(cv))[i]);
    
  case CTYPE_SINT8:
    return make_number_from_long_long(((signed char *)GETCVVECTOR(cv))[i]);
    
  case CTYPE_SINT16:
    return make_number_from_long_long(((signed short *)GETCVVECTOR(cv))[i]);
    
  case CTYPE_SINT32:
    return make_number_from_long_long(((signed long *)GETCVVECTOR(cv))[i]);
    
  case CTYPE_SINT64:
    return make_number_from_long_long(((signed long long *)GETCVVECTOR(cv))[i]);
    
  case CTYPE_UINT8:
    return make_number_from_long_long(((unsigned char *)GETCVVECTOR(cv))[i]);
    
  case CTYPE_UINT16:
    return make_number_from_long_long(((unsigned short *)GETCVVECTOR(cv))[i]);
    
  case CTYPE_UINT32:
    return make_number_from_long_long(((unsigned long *)GETCVVECTOR(cv))[i]);
    
  case CTYPE_UINT64:
    return make_number_from_long_long(((unsigned long long *)GETCVVECTOR(cv))[i]);

  default:
    limo_error("unknown type in cvector");
  }
}

static void init_ctypes(limo_data *env) {
  INSBUILTINVAR(make_number_from_long_long(CTYPE_SINT8),  "_SINT8");
  INSBUILTINVAR(make_number_from_long_long(CTYPE_SINT16), "_SINT16");
  INSBUILTINVAR(make_number_from_long_long(CTYPE_SINT32), "_SINT32");
  INSBUILTINVAR(make_number_from_long_long(CTYPE_SINT64), "_SINT64");
  INSBUILTINVAR(make_number_from_long_long(CTYPE_UINT8),  "_UINT8");
  INSBUILTINVAR(make_number_from_long_long(CTYPE_UINT16), "_UINT16");
  INSBUILTINVAR(make_number_from_long_long(CTYPE_UINT32), "_UINT32");
  INSBUILTINVAR(make_number_from_long_long(CTYPE_UINT64), "_UINT64");
  INSBUILTINVAR(make_number_from_long_long(CTYPE_FLOAT),  "_FLOAT");
  INSBUILTINVAR(make_number_from_long_long(CTYPE_DOUBLE), "_DOUBLE");
}

void limo_dll_init(limo_data *env) 
{
  cvector_type_sym = make_sym("CVECTOR");
  limo_true = make_sym(":T");
  init_ctypes(env);
  INSBUILTIN(builtin_ctype_sizeof, "CTYPE-SIZEOF");
  INSBUILTIN(builtin_make_cvector, "MAKE-CVECTOR");
  INSBUILTIN(builtin_cvectorp, "CVECTORP");
  INSBUILTIN(builtin_cvector_length, "CVECTOR-LENGTH");
  INSBUILTIN(builtin_cvector_type, "CVECTOR-TYPE");
  INSBUILTIN(builtin_cvector_ref_unsafe, "CVECTOR-REF/UNSAFE");
  INSBUILTIN(builtin_cvector_set_unsafe, "CVECTOR-SET/UNSAFE");
}
