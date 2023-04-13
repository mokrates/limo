#include "limo.h"

#include <stdint.h>

// C99 or later
#if UINTPTR_MAX == 0xFFFF
  #define WORD_SHIFT 1
#elif UINTPTR_MAX == 0xFFFFFFFF
  #define WORD_SHIFT 2
#elif UINTPTR_MAX == 0xFFFFFFFFFFFFFFFFu
  #define WORD_SHIFT 3
#else
  #error TBD pointer size
#endif

void *flmalloc(size_t sz)
{
  void *result;
  void **fl = pk_flmalloc_get();

  if (!sz) return NULL;

  int idx = (sz-1) >> WORD_SHIFT;   // 1 WORD on idx=0

  if (idx >= MAX_FLMALLOC_LISTS) {
    return GC_malloc(sz);    // we don't do objects that large
  }
  
  if (!fl[idx])
    fl[idx] = GC_malloc_many((idx+1)<<WORD_SHIFT);

  // let's try without GC_malloc_many
  /* if (!fl[idx]) */
  /*   return GC_malloc((idx+1)<<WORD_SHIFT); */

  result = fl[idx];
  fl[idx] = *(void **)fl[idx];
  return result;
}

void flfree(void *o, size_t sz)
{
  size_t idx;
  void **fl = pk_flmalloc_get();  
  if (sz < (1>>WORD_SHIFT)) return;   // we don't want this small objects
  if (sz > (MAX_FLMALLOC_LISTS-1) << WORD_SHIFT) return;  // .. and this is too large for us

  idx = (sz-1)>>WORD_SHIFT;
  *((void **)o) = fl[idx];
  fl[idx] = o;
}
