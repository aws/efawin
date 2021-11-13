/* GPLv2 or OpenIB.org BSD (MIT) See COPYING file */
#ifndef UTIL_UTIL_H
#define UTIL_UTIL_H

#ifndef _WIN32
#include <ccan/ilog.h>
#endif
#include <stdbool.h>
#include <sys/types.h>
#include <stdio.h>

/* Return true if the snprintf succeeded, false if there was truncation or
 * error */
static inline bool __good_snprintf(size_t len, int rc)
{
	return (rc < len && rc >= 0);
}

#define check_snprintf(buf, len, fmt, ...)                                     \
	__good_snprintf(len, snprintf(buf, len, fmt, ##__VA_ARGS__))

/* a CMP b. See also the BSD macro timercmp(). */
#define ts_cmp(a, b, CMP)			\
	(((a)->tv_sec == (b)->tv_sec) ?		\
	 ((a)->tv_nsec CMP (b)->tv_nsec) :	\
	 ((a)->tv_sec CMP (b)->tv_sec))

#define offsetofend(_type, _member)                                            \
	(offsetof(_type, _member) + sizeof(((_type *)0)->_member))

#define BITS_PER_LONG	(8 * sizeof(long))

#define GENMASK(h, l) \
	(((~0UL) - (1UL << (l)) + 1) & (~0UL >> (BITS_PER_LONG - 1 - (h))))

#define BIT(nr) (1UL << (nr))

#define __bf_shf(x) (__builtin_ffsll(x) - 1)

/**
 * FIELD_PREP() - prepare a bitfield element
 * @_mask: shifted mask defining the field's length and position
 * @_val:  value to put in the field
 *
 * FIELD_PREP() masks and shifts up the value.  The result should
 * be combined with other fields of the bitfield using logical OR.
 */
#define FIELD_PREP(_mask, _val)                                                \
	({                                                                     \
		((typeof(_mask))(_val) << __bf_shf(_mask)) & (_mask);          \
	})
/**
 * FIELD_GET() - extract a bitfield element
 * @_mask: shifted mask defining the field's length and position
 * @_reg:  value of entire bitfield
 *
 * FIELD_GET() extracts the field specified by @_mask from the
 * bitfield passed in as @_reg by masking and shifting it down.
 */
#ifndef _WIN32
#define FIELD_GET(_mask, _reg)                                                 \
	({                                                                     \
		(typeof(_mask))(((_reg) & (_mask)) >> __bf_shf(_mask));        \
	})
#else
#pragma intrinsic(_BitScanForward64)
static inline uint64_t FIELD_GET(uint64_t _mask, uint64_t _reg)
{
    unsigned long index = 0;
    auto result = _BitScanForward64(&index, _mask);
    return (_reg & _mask) >> index;
}
#endif

static inline unsigned long align(unsigned long val, unsigned long align)
{
	return (val + align - 1) & ~(align - 1);
}

static inline uint64_t roundup_pow_of_two(uint64_t n)
{
#ifndef _WIN32
	return n == 1 ? 1 : 1ULL << ilog64(n - 1);
#else
    unsigned long roundup = 1;

    if (n == 1)
	 return (roundup << 1);

    while (roundup < n)
	 roundup <<= 1;

    return roundup;
#endif
}

static inline unsigned long DIV_ROUND_UP(unsigned long n, unsigned long d)
{
	return (n + d - 1) / d;
}

#endif
