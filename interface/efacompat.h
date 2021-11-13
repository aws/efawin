/* SPDX-License-Identifier: GPL-2.0 OR Linux-OpenIB */
/*
 * Copyright Amazon.com, Inc. or its affiliates. All rights reserved.
 *
 * This software is available to you under a choice of one of two
 * licenses.  You may choose to be licensed under the terms of the GNU
 * General Public License (GPL) Version 2, available from the file
 * Linux-OpenIB in the main directory of this source tree, or the
 * OpenIB.org BSD license below:
 *
 *     Redistribution and use in source and binary forms, with or
 *     without modification, are permitted provided that the following
 *     conditions are met:
 *
 *      - Redistributions of source code must retain the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer.
 *
 *      - Redistributions in binary form must reproduce the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer in the documentation and/or other materials
 *        provided with the distribution.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef __EFACOMPAT_H__
#define __EFACOMPAT_H__

#include <inttypes.h>
#include <basetsd.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint8_t __be8;
typedef uint16_t __be16;
typedef uint32_t __be32;
typedef uint64_t __be64;

typedef SSIZE_T ssize_t;

typedef uint8_t __u8;
typedef uint16_t __u16;
typedef uint32_t __u32;
typedef uint64_t __u64;

typedef int32_t __s32;

#ifndef PAGE_SIZE
#define PAGE_SIZE 4096
#endif

#define  NAME_MAX 256
#define  PATH_MAX MAX_PATH
/* This is the expected value returned from the device when querying its attributes.
 * Defining it here, as we need the value at compile time for sizing the data path
 * descriptor array appropriately.
 */
#define EFA_DEV_ATTR_MAX_SQ_SGE 2
#define EFA_DEV_ATTR_MAX_WR_SGE 2

typedef unsigned int uid_t;

#define __aligned_u64 __declspec(align(64)) UINT64

static inline uid_t getuid(void)
{
    // This is a linux command not available on windows, and is used by the EFA provider
    // for addressing when the local and remote peers are on the same node with SHM enabled.
    // Because the provider disables this SHM communication, this function is not called.
    return 0;
}

/*
 * On Windows host is always using little endian
 * call byteswap intrinsics to convert to and from big endian values.
 */
#ifndef htobe32
#define htobe32(value) _byteswap_ulong(value)
#endif
#ifndef be32toh
#define be32toh(value) _byteswap_ulong(value)
#endif

#ifndef min_t
#define min_t(type, a, b) min(a,b)
#endif
#ifndef max_t
#define max_t(type, a, b) max(a,b)
#endif

#define __attribute__(_attribute_name_)
#define __builtin_constant_p(_const_expr_) (_const_expr_)

#ifndef container_of
#define container_of(ptr, type, member) ( (type *) ((char *)ptr - offsetof(type, member)))
#endif

#ifdef __cplusplus
}
#endif

#endif /* __EFACOMPAT_H__ */
