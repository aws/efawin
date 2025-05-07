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

#include <Windows.h>
#include <inttypes.h>
#include <infiniband/verbs.h>
#include <efawinver.h>
HMODULE hEfawinLib = NULL;

/*
 * Dummy functions will let us fail calls gracefully in the event that we are unable to
 * load the functions from efawin.dll
 */
struct ibv_mr* dummy_reg_mr_iova2(struct ibv_pd* pd, void* addr, size_t length,
    uint64_t iova, unsigned int access)
{
    return NULL;
}

#undef ibv_reg_mr
struct ibv_mr* dummy_reg_mr(struct ibv_pd* pd, void* addr, size_t length,
    int access)
{
    return NULL;
}

int dummy_dereg_mr(struct ibv_mr* mr)
{
    return -ENODEV;
}

int dummy_query_device(struct ibv_context* context,
    struct ibv_device_attr* device_attr)
{
    return -ENODEV;
}

struct ibv_qp_ex* dummy_qp_to_qp_ex(struct ibv_qp* qp)
{
    return NULL;
}

#undef ibv_query_port
int dummy_query_port(struct ibv_context* context, uint8_t port_num,
    struct ibv_port_attr* port_attr)
{
    return -ENODEV;
}

int dummy_query_gid(struct ibv_context* context, uint8_t port_num,
    int index, union ibv_gid* gid)
{
    return -ENODEV;
}

int dummy_modify_qp(struct ibv_qp* qp, struct ibv_qp_attr* attr,
    int attr_mask)
{
    return -ENODEV;
}

int dummy_destroy_qp(struct ibv_qp* qp)
{
    return -ENODEV;
}

struct ibv_qp* dummy_create_qp(struct ibv_pd* pd,
    struct ibv_qp_init_attr* qp_init_attr)
{
    return NULL;
}

int dummy_dealloc_pd(struct ibv_pd* pd)
{
    return -ENODEV;
}

struct ibv_pd* dummy_alloc_pd(struct ibv_context* context)
{
    return NULL;
}

int dummy_fork_init(void)
{
    return -ENODEV;
}

void dummy_free_device_list(struct ibv_device** list)
{
    /* Nothing to do */
}

int dummy_close_device(struct ibv_context* context)
{
    return -ENODEV;
}

struct ibv_device** dummy_get_device_list(int* num_devices)
{
    return NULL;
}

struct ibv_context* dummy_open_device(struct ibv_device* device)
{
    return NULL;
}

struct ibv_cq* dummy_create_cq(struct ibv_context* context, int cqe,
    void* cq_context,
    struct ibv_comp_channel* channel,
    int comp_vector)
{
    return NULL;
}

struct ibv_cq* dummy_create_cq_ex(struct ibv_context* context, struct ibv_cq_init_attr_ex *cq_attr)
{
    return NULL;
}

const char* dummy_wc_status_str(enum ibv_wc_status status)
{
    return NULL;
}

int dummy_destroy_cq(struct ibv_cq* cq)
{
    return -ENODEV;
}

int dummy_destroy_ah(struct ibv_ah* ah)
{
    return -ENODEV;
}

struct ibv_ah* dummy_create_ah(struct ibv_pd* pd, struct ibv_ah_attr* attr)
{
    return NULL;
}

int dummy_query_ah(struct ibv_ah* ibvah, struct efadv_ah_attr* attr,
    uint32_t inlen)
{
    return -ENODEV;
}

int dummy_windv_query_device(struct ibv_context* ibvctx,
    struct efadv_device_attr* attr,
    uint32_t inlen)
{
    return -ENODEV;
}

struct ibv_qp* dummy_create_qp_ex(struct ibv_context* ibvctx,
    struct ibv_qp_init_attr_ex* attr_ex,
    struct efadv_qp_init_attr* efa_attr,
    uint32_t inlen)
{
    return NULL;
}

struct efawin_ops {
    efa_win_reg_mr_fn efa_win_reg_mr;
    efa_win_reg_mr_iova2_fn efa_win_reg_mr_iova2;
    efa_win_dereg_mr_fn efa_win_dereg_mr;
    efa_win_query_device_fn efa_win_query_device;
    efa_win_qp_to_qp_ex_fn efa_win_qp_to_qp_ex;
    efa_win_query_port_fn efa_win_query_port;
    efa_win_query_gid_fn efa_win_query_gid;
    efa_win_modify_qp_fn efa_win_modify_qp;
    efa_win_destroy_qp_fn efa_win_destroy_qp;
    efa_win_create_qp_fn efa_win_create_qp;
    efa_win_dealloc_pd_fn efa_win_dealloc_pd;
    efa_win_alloc_pd_fn efa_win_alloc_pd;
    efa_win_fork_init_fn efa_win_fork_init;
    efa_win_free_device_list_fn efa_win_free_device_list;
    efa_win_close_device_fn efa_win_close_device;
    efa_win_get_device_list_fn efa_win_get_device_list;
    efa_win_open_device_fn efa_win_open_device;
    efa_win_create_cq_fn efa_win_create_cq;
    efa_win_create_cq_ex_fn efa_win_create_cq_ex;
    efa_win_wc_status_str_fn efa_win_wc_status_str;
    efa_win_destroy_cq_fn efa_win_destroy_cq;
    efa_win_destroy_ah_fn efa_win_destroy_ah;
    efa_win_create_ah_fn efa_win_create_ah;
    efa_windv_query_ah_fn efa_windv_query_ah;
    efa_windv_query_device_fn efa_windv_query_device;
    efa_windv_create_qp_ex_fn efa_windv_create_qp_ex;
};

struct efawin_ops efawin_ops = {
    .efa_win_reg_mr = dummy_reg_mr,
    .efa_win_reg_mr_iova2 = dummy_reg_mr_iova2,
    .efa_win_dereg_mr = dummy_dereg_mr,
    .efa_win_query_device = dummy_query_device,
    .efa_win_qp_to_qp_ex = dummy_qp_to_qp_ex,
    .efa_win_query_port = dummy_query_port,
    .efa_win_query_gid = dummy_query_gid,
    .efa_win_modify_qp = dummy_modify_qp,
    .efa_win_destroy_qp = dummy_destroy_qp,
    .efa_win_create_qp = dummy_create_qp,
    .efa_win_dealloc_pd = dummy_dealloc_pd,
    .efa_win_alloc_pd = dummy_alloc_pd,
    .efa_win_fork_init = dummy_fork_init,
    .efa_win_free_device_list = dummy_free_device_list,
    .efa_win_close_device = dummy_close_device,
    .efa_win_get_device_list = dummy_get_device_list,
    .efa_win_open_device = dummy_open_device,
    .efa_win_create_cq = dummy_create_cq,
    .efa_win_create_cq_ex = dummy_create_cq_ex,
    .efa_win_wc_status_str = dummy_wc_status_str,
    .efa_win_destroy_cq = dummy_destroy_cq,
    .efa_win_destroy_ah = dummy_destroy_ah,
    .efa_win_create_ah = dummy_create_ah,
    .efa_windv_query_ah = dummy_query_ah,
    .efa_windv_query_device = dummy_windv_query_device,
    .efa_windv_create_qp_ex = dummy_create_qp_ex,
};

void efa_free_efawin_lib()
{
    if(hEfawinLib != NULL) {
        FreeLibrary(hEfawinLib);
        hEfawinLib = NULL;
    }
}

int efa_load_efawin_lib()
{
    efa_win_reg_mr_fn efa_win_reg_mr;
    efa_win_reg_mr_iova2_fn efa_win_reg_mr_iova2;
    efa_win_dereg_mr_fn efa_win_dereg_mr;
    efa_win_query_device_fn efa_win_query_device;
    efa_win_qp_to_qp_ex_fn efa_win_qp_to_qp_ex;
    efa_win_query_port_fn efa_win_query_port;
    efa_win_query_gid_fn efa_win_query_gid;
    efa_win_modify_qp_fn efa_win_modify_qp;
    efa_win_destroy_qp_fn efa_win_destroy_qp;
    efa_win_create_qp_fn efa_win_create_qp;
    efa_win_dealloc_pd_fn efa_win_dealloc_pd;
    efa_win_alloc_pd_fn efa_win_alloc_pd;
    efa_win_fork_init_fn efa_win_fork_init;
    efa_win_free_device_list_fn efa_win_free_device_list;
    efa_win_close_device_fn efa_win_close_device;
    efa_win_get_device_list_fn efa_win_get_device_list;
    efa_win_open_device_fn efa_win_open_device;
    efa_win_create_cq_fn efa_win_create_cq;
    efa_win_create_cq_ex_fn efa_win_create_cq_ex;
    efa_win_wc_status_str_fn efa_win_wc_status_str;
    efa_win_destroy_cq_fn efa_win_destroy_cq;
    efa_win_destroy_ah_fn efa_win_destroy_ah;
    efa_win_create_ah_fn efa_win_create_ah;
    efa_windv_query_ah_fn efa_windv_query_ah;
    efa_windv_query_device_fn efa_windv_query_device;
    efa_windv_create_qp_ex_fn efa_windv_create_qp_ex;
    efa_win_check_version_fn efa_win_check_version;

    if(hEfawinLib != NULL) {
        return -EALREADY;
    }
    hEfawinLib = LoadLibraryA("efawin.dll");
    if (!hEfawinLib) {
        int error = GetLastError();
        return -error;
    }

    efa_win_check_version = (efa_win_check_version_fn)GetProcAddress(hEfawinLib, "check_version");
    if (!efa_win_check_version) {
        goto liberror;
    }

    if (!efa_win_check_version(EFAWIN_INTERFACE_VERSION)) {
        goto liberror;
    }

    efa_win_reg_mr = (efa_win_reg_mr_fn)GetProcAddress(hEfawinLib, "ibv_reg_mr");
    if (!efa_win_reg_mr) {
        goto liberror;
    }
    efa_win_reg_mr_iova2 = (efa_win_reg_mr_iova2_fn)GetProcAddress(hEfawinLib, "ibv_reg_mr_iova2");
    if (!efa_win_reg_mr_iova2) {
        goto liberror;
    }
    efa_win_dereg_mr = (efa_win_dereg_mr_fn)GetProcAddress(hEfawinLib, "ibv_dereg_mr");
    if (!efa_win_dereg_mr) {
        goto liberror;
    }
    efa_win_query_device = (efa_win_query_device_fn)GetProcAddress(hEfawinLib, "ibv_query_device");
    if (!efa_win_query_device) {
        goto liberror;
    }
    efa_win_qp_to_qp_ex = (efa_win_qp_to_qp_ex_fn)GetProcAddress(hEfawinLib, "ibv_qp_to_qp_ex");
    if (!efa_win_qp_to_qp_ex) {
        goto liberror;
    }
    efa_win_query_port = (efa_win_query_port_fn)GetProcAddress(hEfawinLib, "ibv_query_port");
    if (!efa_win_query_port) {
        goto liberror;
    }
    efa_win_query_gid = (efa_win_query_gid_fn)GetProcAddress(hEfawinLib, "ibv_query_gid");
    if (!efa_win_query_gid) {
        goto liberror;
    }
    efa_win_modify_qp = (efa_win_modify_qp_fn)GetProcAddress(hEfawinLib, "ibv_modify_qp");
    if (!efa_win_modify_qp) {
        goto liberror;
    }
    efa_win_destroy_qp = (efa_win_destroy_qp_fn)GetProcAddress(hEfawinLib, "ibv_destroy_qp");
    if (!efa_win_destroy_qp) {
        goto liberror;
    }
    efa_win_create_qp = (efa_win_create_qp_fn)GetProcAddress(hEfawinLib, "ibv_create_qp");
    if (!efa_win_create_qp) {
        goto liberror;
    }
    efa_win_dealloc_pd = (efa_win_dealloc_pd_fn)GetProcAddress(hEfawinLib, "ibv_dealloc_pd");
    if (!efa_win_dealloc_pd) {
        goto liberror;
    }
    efa_win_alloc_pd = (efa_win_alloc_pd_fn)GetProcAddress(hEfawinLib, "ibv_alloc_pd");
    if (!efa_win_alloc_pd) {
        goto liberror;
    }
    efa_win_fork_init = (efa_win_fork_init_fn)GetProcAddress(hEfawinLib, "ibv_fork_init");
    if (!efa_win_fork_init) {
        goto liberror;
    }
    efa_win_free_device_list = (efa_win_free_device_list_fn)GetProcAddress(hEfawinLib, "ibv_free_device_list");
    if (!efa_win_free_device_list) {
        goto liberror;
    }
    efa_win_close_device = (efa_win_close_device_fn)GetProcAddress(hEfawinLib, "ibv_close_device");
    if (!efa_win_close_device) {
        goto liberror;
    }
    efa_win_get_device_list = (efa_win_get_device_list_fn)GetProcAddress(hEfawinLib, "ibv_get_device_list");
    if (!efa_win_get_device_list) {
        goto liberror;
    }
    efa_win_open_device = (efa_win_open_device_fn)GetProcAddress(hEfawinLib, "ibv_open_device");
    if (!efa_win_open_device) {
        goto liberror;
    }
    efa_win_create_cq = (efa_win_create_cq_fn)GetProcAddress(hEfawinLib, "ibv_create_cq");
    if (!efa_win_create_cq) {
        goto liberror;
    }

    efa_win_create_cq_ex = (efa_win_create_cq_ex_fn)GetProcAddress(hEfawinLib, "ibv_create_cq_ex");
    if (!efa_win_create_cq_ex) {
        goto liberror;
    }

    efa_win_wc_status_str = (efa_win_wc_status_str_fn)GetProcAddress(hEfawinLib, "ibv_wc_status_str");
    if (!efa_win_wc_status_str) {
        goto liberror;
    }
    efa_win_destroy_cq = (efa_win_destroy_cq_fn)GetProcAddress(hEfawinLib, "ibv_destroy_cq");
    if (!efa_win_destroy_cq) {
        goto liberror;
    }
    efa_win_destroy_ah = (efa_win_destroy_ah_fn)GetProcAddress(hEfawinLib, "ibv_destroy_ah");
    if (!efa_win_destroy_ah) {
        goto liberror;
    }
    efa_win_create_ah = (efa_win_create_ah_fn)GetProcAddress(hEfawinLib, "ibv_create_ah");
    if (!efa_win_create_ah) {
        goto liberror;
    }

    efa_windv_query_ah = (efa_windv_query_ah_fn)GetProcAddress(hEfawinLib, "efadv_query_ah");
    if (!efa_windv_query_ah) {
        goto liberror;
    }
    efa_windv_create_qp_ex = (efa_windv_create_qp_ex_fn)GetProcAddress(hEfawinLib, "efadv_create_qp_ex");
    if (!efa_windv_create_qp_ex) {
        goto liberror;
    }
    efa_windv_query_device = (efa_windv_query_device_fn)GetProcAddress(hEfawinLib, "efadv_query_device");
    if (!efa_windv_query_device) {
        goto liberror;
    }

    efawin_ops.efa_win_reg_mr = efa_win_reg_mr;
    efawin_ops.efa_win_reg_mr_iova2 = efa_win_reg_mr_iova2;
    efawin_ops.efa_win_dereg_mr = efa_win_dereg_mr;
    efawin_ops.efa_win_query_device = efa_win_query_device;
    efawin_ops.efa_win_qp_to_qp_ex = efa_win_qp_to_qp_ex;
    efawin_ops.efa_win_query_port = efa_win_query_port;
    efawin_ops.efa_win_query_gid = efa_win_query_gid;
    efawin_ops.efa_win_modify_qp = efa_win_modify_qp;
    efawin_ops.efa_win_destroy_qp = efa_win_destroy_qp;
    efawin_ops.efa_win_create_qp = efa_win_create_qp;
    efawin_ops.efa_win_dealloc_pd = efa_win_dealloc_pd;
    efawin_ops.efa_win_alloc_pd = efa_win_alloc_pd;
    efawin_ops.efa_win_fork_init = efa_win_fork_init;
    efawin_ops.efa_win_free_device_list = efa_win_free_device_list;
    efawin_ops.efa_win_close_device = efa_win_close_device;
    efawin_ops.efa_win_get_device_list = efa_win_get_device_list;
    efawin_ops.efa_win_open_device = efa_win_open_device;
    efawin_ops.efa_win_create_cq = efa_win_create_cq;
    efawin_ops.efa_win_create_cq_ex = efa_win_create_cq_ex;
    efawin_ops.efa_win_wc_status_str = efa_win_wc_status_str;
    efawin_ops.efa_win_destroy_cq = efa_win_destroy_cq;
    efawin_ops.efa_win_destroy_ah = efa_win_destroy_ah;
    efawin_ops.efa_win_create_ah = efa_win_create_ah;
    efawin_ops.efa_windv_query_ah = efa_windv_query_ah;
    efawin_ops.efa_windv_query_device = efa_windv_query_device;
    efawin_ops.efa_windv_create_qp_ex = efa_windv_create_qp_ex;

    return ERROR_SUCCESS;
    
liberror:
    if (hEfawinLib)
    {
        FreeLibrary(hEfawinLib);
        hEfawinLib = NULL;
    }
    return -ENODEV;
}

struct ibv_mr* ibv_reg_mr_iova2(struct ibv_pd* pd, void* addr, size_t length,
    uint64_t iova, unsigned int access)
{
    return efawin_ops.efa_win_reg_mr_iova2(pd, addr, length, iova, access);
}

#undef ibv_reg_mr
struct ibv_mr* ibv_reg_mr(struct ibv_pd* pd, void* addr, size_t length,
    int access)
{
    return efawin_ops.efa_win_reg_mr(pd, addr, length, access);
}

int ibv_dereg_mr(struct ibv_mr* mr)
{
    return efawin_ops.efa_win_dereg_mr(mr);
}

int ibv_query_device(struct ibv_context* context,
    struct ibv_device_attr* device_attr)
{
    return efawin_ops.efa_win_query_device(context, device_attr);
}

struct ibv_qp_ex* ibv_qp_to_qp_ex(struct ibv_qp* qp)
{
    return efawin_ops.efa_win_qp_to_qp_ex(qp);
}

#undef ibv_query_port
int ibv_query_port(struct ibv_context* context, uint8_t port_num,
    struct _compat_ibv_port_attr* port_attr)
{
    return efawin_ops.efa_win_query_port(context, port_num, (struct ibv_port_attr *)port_attr);
}

int ibv_query_gid(struct ibv_context* context, uint8_t port_num,
    int index, union ibv_gid* gid)
{
    return efawin_ops.efa_win_query_gid(context, port_num, index, gid);
}

int ibv_modify_qp(struct ibv_qp* qp, struct ibv_qp_attr* attr,
    int attr_mask)
{
    return efawin_ops.efa_win_modify_qp(qp, attr, attr_mask);
}

int ibv_destroy_qp(struct ibv_qp* qp)
{
    return efawin_ops.efa_win_destroy_qp(qp);
}

struct ibv_qp* ibv_create_qp(struct ibv_pd* pd,
    struct ibv_qp_init_attr* qp_init_attr)
{
    return efawin_ops.efa_win_create_qp(pd, qp_init_attr);
}

int ibv_dealloc_pd(struct ibv_pd* pd)
{
    return efawin_ops.efa_win_dealloc_pd(pd);
}

struct ibv_pd* ibv_alloc_pd(struct ibv_context* context)
{
    return efawin_ops.efa_win_alloc_pd(context);
}

int ibv_fork_init(void)
{
    return efawin_ops.efa_win_fork_init();
}

void ibv_free_device_list(struct ibv_device** list)
{
    efawin_ops.efa_win_free_device_list(list);
}

int ibv_close_device(struct ibv_context* context)
{
    return efawin_ops.efa_win_close_device(context);
}

struct ibv_device** ibv_get_device_list(int* num_devices)
{
    return efawin_ops.efa_win_get_device_list(num_devices);
}

struct ibv_context* ibv_open_device(struct ibv_device* device)
{
    return efawin_ops.efa_win_open_device(device);
}

struct ibv_cq* ibv_create_cq(struct ibv_context* context, int cqe,
    void* cq_context,
    struct ibv_comp_channel* channel,
    int comp_vector)
{
    return efawin_ops.efa_win_create_cq(context, cqe, cq_context, channel, comp_vector);
}

struct ibv_cq_ex *ibv_create_cq_ex(struct ibv_context* context, struct ibv_cq_init_attr_ex *cq_attr)
{
    return efawin_ops.efa_win_create_cq_ex(context, cq_attr);
}

const char* ibv_wc_status_str(enum ibv_wc_status status)
{
    return efawin_ops.efa_win_wc_status_str(status);
}

int ibv_destroy_cq(struct ibv_cq* cq)
{
    return efawin_ops.efa_win_destroy_cq(cq);
}

int ibv_destroy_ah(struct ibv_ah* ah)
{
    return efawin_ops.efa_win_destroy_ah(ah);
}

struct ibv_ah* ibv_create_ah(struct ibv_pd* pd, struct ibv_ah_attr* attr)
{
    return efawin_ops.efa_win_create_ah(pd, attr);
}

int efadv_query_ah(struct ibv_ah* ibvah, struct efadv_ah_attr* attr,
    uint32_t inlen)
{
    return efawin_ops.efa_windv_query_ah(ibvah, attr, inlen);
}

int efadv_query_device(struct ibv_context* ibvctx,
    struct efadv_device_attr* attr,
    uint32_t inlen)
{
    return efawin_ops.efa_windv_query_device(ibvctx, attr, inlen);
}

struct ibv_qp* efadv_create_qp_ex(struct ibv_context* ibvctx,
    struct ibv_qp_init_attr_ex* attr_ex,
    struct efadv_qp_init_attr* efa_attr,
    uint32_t inlen)
{
    return efawin_ops.efa_windv_create_qp_ex(ibvctx, attr_ex, efa_attr, inlen);
}
