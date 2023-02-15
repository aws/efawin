/* SPDX-License-Identifier: GPL-2.0 OR Linux-OpenIB */
/*
 * Copyright Amazon.com, Inc. or its affiliates. All rights reserved.
 */

#ifndef __EFAWINVER_H__
#define __EFAWINVER_H__

#include<stdbool.h>

#define EFAWIN_INTERFACE_VERSION 1

/*
 * List of functions types for the infiniband functions supported by efawin dll.
 */

typedef bool (*efa_win_check_version_fn)(int interface_version);

typedef struct ibv_mr* (*efa_win_reg_mr_fn)(struct ibv_pd* pd, void* addr, size_t length,
    int access);

typedef struct ibv_mr* (*efa_win_reg_mr_iova2_fn)(struct ibv_pd* pd, void* addr, size_t length,
    uint64_t iova, unsigned int access);

typedef int (*efa_win_dereg_mr_fn)(struct ibv_mr* mr);

typedef int (*efa_win_query_device_fn)(struct ibv_context* context,
    struct ibv_device_attr* device_attr);

typedef struct ibv_qp_ex* (*efa_win_qp_to_qp_ex_fn)(struct ibv_qp* qp);

typedef int (*efa_win_query_port_fn)(struct ibv_context* context, uint8_t port_num,
    struct ibv_port_attr* port_attr);

typedef int (*efa_win_query_gid_fn)(struct ibv_context* context, uint8_t port_num,
    int index, union ibv_gid* gid);

typedef int (*efa_win_modify_qp_fn)(struct ibv_qp* qp, struct ibv_qp_attr* attr,
    int attr_mask);

typedef int (*efa_win_destroy_qp_fn)(struct ibv_qp* qp);

typedef struct ibv_qp* (*efa_win_create_qp_fn)(struct ibv_pd* pd,
    struct ibv_qp_init_attr* qp_init_attr);

typedef int (*efa_win_dealloc_pd_fn)(struct ibv_pd* pd);

typedef struct ibv_pd* (*efa_win_alloc_pd_fn)(struct ibv_context* context);

typedef int (*efa_win_fork_init_fn)(void);

typedef void (*efa_win_free_device_list_fn)(struct ibv_device** list);

typedef int (*efa_win_close_device_fn)(struct ibv_context* context);

typedef struct ibv_device** (*efa_win_get_device_list_fn)(int* num_devices);

typedef struct ibv_context* (*efa_win_open_device_fn)(struct ibv_device* device);

typedef struct ibv_cq* (*efa_win_create_cq_fn)(struct ibv_context* context, int cqe,
    void* cq_context,
    struct ibv_comp_channel* channel,
    int comp_vector);

typedef struct ibv_cq_ex* (*efa_win_create_cq_ex_fn)(struct ibv_context* context, struct ibv_cq_init_attr_ex *cq_attr);

typedef const char* (*efa_win_wc_status_str_fn)(enum ibv_wc_status status);

typedef int (*efa_win_destroy_cq_fn)(struct ibv_cq* cq);

typedef int (*efa_win_destroy_ah_fn)(struct ibv_ah* ah);

typedef struct ibv_ah* (*efa_win_create_ah_fn)(struct ibv_pd* pd, struct ibv_ah_attr* attr);

typedef int (*efa_windv_query_ah_fn)(struct ibv_ah* ibvah, struct efadv_ah_attr* attr,
    uint32_t inlen);

typedef int (*efa_windv_query_device_fn)(struct ibv_context* ibvctx,
    struct efadv_device_attr* attr,
    uint32_t inlen);

typedef struct ibv_qp* (*efa_windv_create_qp_ex_fn)(struct ibv_context* ibvctx,
    struct ibv_qp_init_attr_ex* attr_ex,
    struct efadv_qp_init_attr* efa_attr,
    uint32_t inlen);

#endif
