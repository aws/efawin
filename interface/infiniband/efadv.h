/* SPDX-License-Identifier: GPL-2.0 OR Linux-OpenIB */
/*
 * Copyright Amazon.com, Inc. or its affiliates. All rights reserved.
 *
 * This software is available to you under a choice of one of two
 * licenses.  You may choose to be licensed under the terms of the GNU
 * General Public License (GPL) Version 2, available from the file
 * Linux-OpenIB in the main directory of this source tree, or the
 * BSD license below:
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

#ifndef __EFADV_H__
#define __EFADV_H__

#include <stdio.h>
#include <sys/types.h>
#include <infiniband/verbs.h>

#ifdef __cplusplus
extern "C" {
#endif

    enum {
	 /* Values must match the values in efa-abi.h */
	 EFADV_QP_DRIVER_TYPE_SRD = 0,
    };

    struct ibv_qp* efadv_create_driver_qp(struct ibv_pd* ibvpd,
	 struct ibv_qp_init_attr* attr,
	 uint32_t driver_qp_type);

    struct efadv_qp_init_attr {
	 uint64_t comp_mask;
	 uint32_t driver_qp_type;
	 uint8_t reserved[4];
    };

    struct ibv_qp* efadv_create_qp_ex(struct ibv_context* ibvctx,
	 struct ibv_qp_init_attr_ex* attr_ex,
	 struct efadv_qp_init_attr* efa_attr,
	 uint32_t inlen);

    enum {
	 EFADV_DEVICE_ATTR_CAPS_RDMA_READ = 1 << 0,
	 EFADV_DEVICE_ATTR_CAPS_RNR_RETRY = 1 << 1,
    };

    struct efadv_device_attr {
	 uint64_t comp_mask;
	 uint32_t max_sq_wr;
	 uint32_t max_rq_wr;
	 uint16_t max_sq_sge;
	 uint16_t max_rq_sge;
	 uint16_t inline_buf_size;
	 uint8_t reserved[2];
	 uint32_t device_caps;
	 uint32_t max_rdma_size;
    };

    int efadv_query_device(struct ibv_context* ibvctx,
	 struct efadv_device_attr* attr,
	 uint32_t inlen);

    struct efadv_ah_attr {
	 uint64_t comp_mask;
	 uint16_t ahn;
	 uint8_t reserved[6];
    };

    int efadv_query_ah(struct ibv_ah* ibvah, struct efadv_ah_attr* attr,
	 uint32_t inlen);

#ifdef __cplusplus
}
#endif

#endif /* __EFADV_H__ */
