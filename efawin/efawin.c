/*
 * Copyright Amazon.com, Inc. or its affiliates. All rights reserved.
 *
 * This software is available to you under a choice of one of two
 * licenses.  You may choose to be licensed under the terms of the GNU
 * General Public License (GPL) Version 2, available from the file
 * LICENSE-GPL-2.0 in the main directory of this source tree, or the
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

#include <stdint.h>
#include <windows.h>
#include <efa_win_cmd.h>
#include <efawinver.h>
#include "efacompat.h"

extern const struct verbs_context_ops efa_ctx_ops;

struct ibv_mr* ibv_reg_mr_iova2(struct ibv_pd* pd, void* addr, size_t length,
    uint64_t iova, unsigned int access)
{
    return efa_ctx_ops.reg_mr(pd, addr, length, iova, access);
}

struct ibv_mr* _ibv_reg_mr(struct ibv_pd* pd, void* addr, size_t length,
    int access)
{
    return efa_ctx_ops.reg_mr(pd, addr, length, 0, access);
}

int ibv_dereg_mr(struct ibv_mr* mr)
{
    return efa_ctx_ops.dereg_mr(container_of(mr, struct verbs_mr, ibv_mr));
}

int ibv_query_device(struct ibv_context* context,
    struct ibv_device_attr* device_attr)
{
    return efa_cmd_query_device(to_efa_context(context), device_attr);
}

struct ibv_qp_ex* ibv_qp_to_qp_ex(struct ibv_qp* qp)
{
    struct verbs_qp* qpx = (struct verbs_qp*)qp;
    return &qpx->qp_ex;
}

int _ibv_query_port(struct ibv_context* context, uint8_t port_num,
    struct ibv_port_attr* port_attr)
{
    return efa_ctx_ops.query_port(context, port_num, port_attr);
}

int ibv_query_gid(struct ibv_context* context, uint8_t port_num,
    int index, union ibv_gid* gid)
{
    return efa_cmd_query_gid(to_efa_context(context), port_num, index, gid);
}

int ibv_modify_qp(struct ibv_qp* qp, struct ibv_qp_attr* attr,
    int attr_mask)
{
    return efa_ctx_ops.modify_qp(qp, attr, attr_mask);
}

int ibv_destroy_qp(struct ibv_qp* qp)
{
    return efa_ctx_ops.destroy_qp(qp);
}

struct ibv_qp* ibv_create_qp(struct ibv_pd* pd,
    struct ibv_qp_init_attr* qp_init_attr)
{
    return efa_ctx_ops.create_qp(pd,qp_init_attr);
}

int ibv_dealloc_pd(struct ibv_pd* pd)
{
    return efa_ctx_ops.dealloc_pd(pd);
}

struct ibv_pd* ibv_alloc_pd(struct ibv_context* context)
{
    return efa_ctx_ops.alloc_pd(context);
}

int ibv_fork_init(void)
{
    return E_FAIL;
}

int ibv_close_device(struct ibv_context* context)
{
    return efa_device_close(to_efa_context(context));
}

struct ibv_context* ibv_open_device(struct ibv_device* device)
{
    return &efa_device_open(device)->ibvctx.context;
}

struct ibv_cq* ibv_create_cq(struct ibv_context* context, int cqe,
    void* cq_context,
    struct ibv_comp_channel* channel,
    int comp_vector)
{
    struct ibv_cq* cq = efa_ctx_ops.create_cq(context, cqe, channel, comp_vector);
    cq->cq_context = cq_context;
    return cq;
}

struct ibv_cq_ex* ibv_create_cq_ex(struct ibv_context* context, struct ibv_cq_init_attr_ex *cq_attr)
{
    struct ibv_cq_ex* cq = efa_ctx_ops.create_cq_ex(context, cq_attr);
    cq->cq_context = context;
    return cq;
}

const char* ibv_wc_status_str(enum ibv_wc_status status)
{
    // WC status mapping is not supported
    return NULL;
}

int ibv_destroy_cq(struct ibv_cq* cq)
{
    return efa_ctx_ops.destroy_cq(cq);
}

int ibv_destroy_ah(struct ibv_ah* ah)
{
    return efa_ctx_ops.destroy_ah(ah);
}

struct ibv_ah* ibv_create_ah(struct ibv_pd* pd, struct ibv_ah_attr* attr)
{
    return efa_ctx_ops.create_ah(pd, attr);
}

bool check_version(int interface_version) {
    return interface_version == EFAWIN_INTERFACE_VERSION;
}

BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,  // handle to DLL module
    DWORD fdwReason,     // reason for calling function
    LPVOID lpReserved)  // reserved
{
    // No initialization on dll load - always return TRUE to indicate success.
    return TRUE;
}
