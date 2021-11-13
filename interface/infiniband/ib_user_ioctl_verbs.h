/* SPDX-License-Identifier: ((GPL-2.0 WITH Linux-syscall-note) OR Linux-OpenIB) */
/*
 * Copyright (c) 2017-2018, Mellanox Technologies inc.  All rights reserved.
 *
 * File truncated from original.
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

#ifndef IB_USER_IOCTL_VERBS_H
#define IB_USER_IOCTL_VERBS_H

#ifndef RDMA_UAPI_PTR
#define RDMA_UAPI_PTR(_type, _name)     __aligned_u64 _name
#endif

#define IB_UVERBS_ACCESS_OPTIONAL_FIRST (1 << 20)
#define IB_UVERBS_ACCESS_OPTIONAL_LAST (1 << 29)

enum ib_uverbs_core_support {
    IB_UVERBS_CORE_SUPPORT_OPTIONAL_MR_ACCESS = 1 << 0,
};

enum ib_uverbs_access_flags {
    IB_UVERBS_ACCESS_LOCAL_WRITE = 1 << 0,
    IB_UVERBS_ACCESS_REMOTE_WRITE = 1 << 1,
    IB_UVERBS_ACCESS_REMOTE_READ = 1 << 2,
    IB_UVERBS_ACCESS_REMOTE_ATOMIC = 1 << 3,
    IB_UVERBS_ACCESS_MW_BIND = 1 << 4,
    IB_UVERBS_ACCESS_ZERO_BASED = 1 << 5,
    IB_UVERBS_ACCESS_ON_DEMAND = 1 << 6,
    IB_UVERBS_ACCESS_HUGETLB = 1 << 7,

    IB_UVERBS_ACCESS_RELAXED_ORDERING = IB_UVERBS_ACCESS_OPTIONAL_FIRST,
    IB_UVERBS_ACCESS_OPTIONAL_RANGE =
    ((IB_UVERBS_ACCESS_OPTIONAL_LAST << 1) - 1) &
    ~(IB_UVERBS_ACCESS_OPTIONAL_FIRST - 1)
};

enum ib_uverbs_srq_type {
    IB_UVERBS_SRQT_BASIC,
    IB_UVERBS_SRQT_XRC,
    IB_UVERBS_SRQT_TM,
};

enum ib_uverbs_wq_type {
    IB_UVERBS_WQT_RQ,
};

enum ib_uverbs_wq_flags {
    IB_UVERBS_WQ_FLAGS_CVLAN_STRIPPING = 1 << 0,
    IB_UVERBS_WQ_FLAGS_SCATTER_FCS = 1 << 1,
    IB_UVERBS_WQ_FLAGS_DELAY_DROP = 1 << 2,
    IB_UVERBS_WQ_FLAGS_PCI_WRITE_END_PADDING = 1 << 3,
};

enum ib_uverbs_qp_type {
    IB_UVERBS_QPT_RC = 2,
    IB_UVERBS_QPT_UC,
    IB_UVERBS_QPT_UD,
    IB_UVERBS_QPT_RAW_PACKET = 8,
    IB_UVERBS_QPT_XRC_INI,
    IB_UVERBS_QPT_XRC_TGT,
    IB_UVERBS_QPT_DRIVER = 0xFF,
};

enum ib_uverbs_qp_create_flags {
    IB_UVERBS_QP_CREATE_BLOCK_MULTICAST_LOOPBACK = 1 << 1,
    IB_UVERBS_QP_CREATE_SCATTER_FCS = 1 << 8,
    IB_UVERBS_QP_CREATE_CVLAN_STRIPPING = 1 << 9,
    IB_UVERBS_QP_CREATE_PCI_WRITE_END_PADDING = 1 << 11,
    IB_UVERBS_QP_CREATE_SQ_SIG_ALL = 1 << 12,
};

enum ib_uverbs_query_port_cap_flags {
    IB_UVERBS_PCF_SM = 1 << 1,
    IB_UVERBS_PCF_NOTICE_SUP = 1 << 2,
    IB_UVERBS_PCF_TRAP_SUP = 1 << 3,
    IB_UVERBS_PCF_OPT_IPD_SUP = 1 << 4,
    IB_UVERBS_PCF_AUTO_MIGR_SUP = 1 << 5,
    IB_UVERBS_PCF_SL_MAP_SUP = 1 << 6,
    IB_UVERBS_PCF_MKEY_NVRAM = 1 << 7,
    IB_UVERBS_PCF_PKEY_NVRAM = 1 << 8,
    IB_UVERBS_PCF_LED_INFO_SUP = 1 << 9,
    IB_UVERBS_PCF_SM_DISABLED = 1 << 10,
    IB_UVERBS_PCF_SYS_IMAGE_GUID_SUP = 1 << 11,
    IB_UVERBS_PCF_PKEY_SW_EXT_PORT_TRAP_SUP = 1 << 12,
    IB_UVERBS_PCF_EXTENDED_SPEEDS_SUP = 1 << 14,
    IB_UVERBS_PCF_CM_SUP = 1 << 16,
    IB_UVERBS_PCF_SNMP_TUNNEL_SUP = 1 << 17,
    IB_UVERBS_PCF_REINIT_SUP = 1 << 18,
    IB_UVERBS_PCF_DEVICE_MGMT_SUP = 1 << 19,
    IB_UVERBS_PCF_VENDOR_CLASS_SUP = 1 << 20,
    IB_UVERBS_PCF_DR_NOTICE_SUP = 1 << 21,
    IB_UVERBS_PCF_CAP_MASK_NOTICE_SUP = 1 << 22,
    IB_UVERBS_PCF_BOOT_MGMT_SUP = 1 << 23,
    IB_UVERBS_PCF_LINK_LATENCY_SUP = 1 << 24,
    IB_UVERBS_PCF_CLIENT_REG_SUP = 1 << 25,
    /*
     * IsOtherLocalChangesNoticeSupported is aliased by IP_BASED_GIDS and
     * is inaccessible
     */
     IB_UVERBS_PCF_LINK_SPEED_WIDTH_TABLE_SUP = 1 << 27,
     IB_UVERBS_PCF_VENDOR_SPECIFIC_MADS_TABLE_SUP = 1 << 28,
     IB_UVERBS_PCF_MCAST_PKEY_TRAP_SUPPRESSION_SUP = 1 << 29,
     IB_UVERBS_PCF_MCAST_FDB_TOP_SUP = 1 << 30,
     IB_UVERBS_PCF_HIERARCHY_INFO_SUP = 1ULL << 31,

     /* NOTE this is an internal flag, not an IBA flag */
     IB_UVERBS_PCF_IP_BASED_GIDS = 1 << 26,
};

enum ib_uverbs_query_port_flags {
    IB_UVERBS_QPF_GRH_REQUIRED = 1 << 0,
};

#endif