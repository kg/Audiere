#ifndef _MM_INLINE_H_
#define _MM_INLINE_H_

#include "mmio.h"
#include <string.h>

static void __inline _mminline_memcpy_long(void *dst, const void *src, int count)
{
    register ULONG *dest = (ULONG *)dst, *srce = (ULONG *)src, i;
    for(i=count; i; i--, dest++, srce++) *dest = *srce;
}


static void __inline _mminline_memcpy_word(void *dst, const void *src, int count)
{
    memcpy(dst, src, count*2);
}

static void __inline _mminline_memcpy_quad(void *dst, const void *src, int count)
{
    register ULONG *dest = (ULONG *)dst, *srce = (ULONG *)src, i;
    for(i=count*2; i; i--, dest++, srce++) *dest = *srce;
}


#endif
