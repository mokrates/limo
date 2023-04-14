#include "limo.h"

#include <stdint.h>

#define MIN(a,b) ((a)<(b)?(a):(b))

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


////////////////////////////////////////////////////////////////////////
// so this allocation story is kinda complicated
// the first thing is, we have buckets of sizes, but we don't have all sizes but only
// multiples of WORD_SIZE (on 64 bit: 8, 16, 24, 32, etc...)
// so if we're to alloc 12 bytes, we can't do that as we don't have a bucket for that
// so we alloc 16 bytes, (from the 16 byte buffer)
// which works. the caller gets too much, which isn't a problem.
//
// but then, the 12 bytes are to be flfreed:
// as we wan't to be agile and agnostic where the memory comes from
// we have to assume that 12 bytes are really only 12 bytes
// so we can't put the memory slice in the 16 bytes bucket, but
// have to put it in the 8 byte bucket.
//
// now we have a problem. if someone in a loop flallocates and flfrees 12 bytes
// we get a memory leak

// so, we try two flfree-functions:
// the SECOND flfree-function has the precondition that the memory was
// allocated with flmalloc which means that the 12 byte block is *actually*
// a 16 byte block, and puts it back there. this way we eliminate the mem leak
// also, this is brittle.

void *flmalloc(size_t sz)
{
  void *result;
  void **fl = pk_flmalloc_get();

  if (!sz) return NULL;

  int idx = (sz-1)>>WORD_SHIFT;  

  if (idx >= MAX_FLMALLOC_LISTS)
    return GC_malloc(sz);    // we don't do objects that large
  
  if (!fl[idx])
    fl[idx] = GC_malloc_many((idx+1)<<WORD_SHIFT);

  // let's try without GC_malloc_many
  /* if (!fl[idx]) */
  /*   return GC_malloc((idx+1)<<WORD_SHIFT); */

  //printf("allocating %ul bytes, idx %ul\n", sz, idx);

  result = fl[idx];
  fl[idx] = *(void **)fl[idx];
  return result;
}

//// SAFE but LEAKY implementation
/* void flfree(void *o, size_t sz) */
/* { */
/*   size_t idx; */
/*   void **fl = pk_flmalloc_get(); */
/*   if (sz < (1<<WORD_SHIFT)) return;   // we don't want this small objects */
/*   if (sz > (MAX_FLMALLOC_LISTS-1) << WORD_SHIFT) return;  // .. and this is too large for us */

/*   idx = (sz-(1<<WORD_SHIFT))>>WORD_SHIFT; */
/*   //printf("freeing %ul bytes, idx %ul\n", sz, idx); */
/*   *((void **)o) = fl[idx]; */
/*   fl[idx] = o; */
/* } */

/* //// UNSAFE but should avoid leaks */
/* //// precondition is: memory was alloc'd with flmalloc! */
void flfree(void *o, size_t sz)
{
  size_t idx;
  void **fl = pk_flmalloc_get();

  idx = (sz-1)>>WORD_SHIFT;

  if (idx >= MAX_FLMALLOC_LISTS)
    return;    // too large for us, let GC handle this

  *((void **)o) = fl[idx];
  fl[idx] = o;
}


void *flrealloc(void *old_ptr, size_t old_size, size_t new_size)
{
  void *new_ptr = flmalloc(new_size);
  memcpy(new_ptr, old_ptr, MIN(old_size, new_size));
  flfree(old_ptr, old_size);
  return new_ptr;
}
