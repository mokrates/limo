#define INSBUILTIN(f, name)    setq(env, make_sym(name), make_builtin(f))
#define INSBUILTINVAR(v, name) setq(env, make_sym(name), v)
#define GETCVTYPE(cv)          (((cvector *)(CDR((cv)->data.d_special) -> data.d_special_intern))->type)
#define GETCVLENGTH(cv)        (((cvector *)(CDR((cv)->data.d_special) -> data.d_special_intern))->length)
#define GETCVVECTOR(cv)        (((cvector *)(CDR((cv)->data.d_special) -> data.d_special_intern))->vector)

typedef struct CVECTOR {
  int type;
  int length;
  void *vector;
} cvector;

#define CTYPE_SINT8  1
#define CTYPE_UINT8  2
#define CTYPE_SINT16 3
#define CTYPE_UINT16 4
#define CTYPE_SINT32 5
#define CTYPE_UINT32 6
#define CTYPE_SINT64 7
#define CTYPE_UINT64 8
#define CTYPE_FLOAT  9
#define CTYPE_DOUBLE 10
