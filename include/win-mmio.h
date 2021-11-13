/* GPLv2 or OpenIB.org BSD (MIT) See COPYING file */

#ifndef __WINMMIO_H__
#define __WINMMIO_H__

#include "efacompat.h"

static inline void mmio_write32(void *addr, uint32_t value)
{
    *(uint32_t*)addr = value;
}

static inline void mmio_write64(void *addr, uint64_t value)
{
    *(uint64_t*)addr = value;
}

#define mmio_write64_be(addr,val) mmio_write64(addr,val)
#define mmio_write32_be(addr,val) mmio_write32(addr,val)

/* Transfer is some multiple of 64 bytes */
static inline void mmio_memcpy_x64(void* dest, const void* src, size_t bytecnt)
{
    uintptr_t* dst_p = dest;

    /* Caller must guarantee:
	 assert(bytecnt != 0);
	 assert((bytecnt % 64) == 0);
	 assert(((uintptr_t)dest) % __alignof__(*dst) == 0);
	 assert(((uintptr_t)src) % __alignof__(*dst) == 0);
    */

    /* Use the native word size for the copy */
    if (sizeof(*dst_p) == 8) {
	 const __be64* src_p = src;

	 do {
	     /* Do 64 bytes at a time */
	     mmio_write64_be(dst_p++, *src_p++);
	     mmio_write64_be(dst_p++, *src_p++);
	     mmio_write64_be(dst_p++, *src_p++);
	     mmio_write64_be(dst_p++, *src_p++);
	     mmio_write64_be(dst_p++, *src_p++);
	     mmio_write64_be(dst_p++, *src_p++);
	     mmio_write64_be(dst_p++, *src_p++);
	     mmio_write64_be(dst_p++, *src_p++);

	     bytecnt -= 8 * sizeof(*dst_p);
	 } while (bytecnt > 0);
    }
    else if (sizeof(*dst_p) == 4) {
	 const __be32* src_p = src;

	 do {
	     mmio_write32_be(dst_p++, *src_p++);
	     mmio_write32_be(dst_p++, *src_p++);
	     bytecnt -= 2 * sizeof(*dst_p);
	 } while (bytecnt > 0);
    }
}

#endif