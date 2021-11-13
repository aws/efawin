/*
 * Copyright (c) 2005 Topspin Communications.  All rights reserved.
 * Copyright (c) 2005, 2006 Cisco Systems.  All rights reserved.
 * Copyright (c) 2005 PathScale, Inc.  All rights reserved.
 * Modifications Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
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

#ifndef __WINIBV_H__
#define __WINIBV_H__

#pragma pack(push,1)
struct ex_hdr {
    struct ib_uverbs_cmd_hdr hdr;
    struct ib_uverbs_ex_cmd_hdr ex_hdr;
};

/*
 * These macros expand to type names that refer to the ABI structure type
 * associated with the given enum string.
 */
#define IBV_ABI_REQ(_enum) _ABI_REQ_STRUCT_##_enum
#define IBV_KABI_REQ(_enum) _KABI_REQ_STRUCT_##_enum
#define IBV_KABI_RESP(_enum) _KABI_RESP_STRUCT_##_enum

#define IBV_ABI_ALIGN(_enum) _ABI_ALIGN_##_enum

 /*
  * Historically the code had copied the data in the kernel headers, modified
  * it and placed them in structs.  To avoid recoding eveything we continue to
  * preserve the same struct layout, with the kernel struct 'loose' inside the
  * modified userspace struct.
  *
  * This is automated with the make_abi_structs.py script which produces the
  * _STRUCT_xx macro that produces a tagless version of the kernel struct. The
  * tagless struct produces a layout that matches the original code.
  */
#define DECLARE_CMDX(_enum, _name, _kabi, _kabi_resp)                          \
	struct _name {                                                         \
		struct ib_uverbs_cmd_hdr hdr;                                  \
		struct _kabi core_payload;                             \
	};                                                                     \
	typedef struct _name IBV_ABI_REQ(_enum);                               \
	typedef struct _kabi IBV_KABI_REQ(_enum);                              \
	typedef struct _kabi_resp IBV_KABI_RESP(_enum);                        \
	enum { IBV_ABI_ALIGN(_enum) = 4 };                                     \

#define DECLARE_CMD(_enum, _name, _kabi)                                       \
	DECLARE_CMDX(_enum, _name, _kabi, _kabi##_resp)

#define DECLARE_CMD_EXX(_enum, _name, _kabi, _kabi_resp)                       \
	struct _name {                                                         \
		struct ex_hdr hdr;                                             \
              struct _kabi core_payload;                             \
	};\
	typedef struct _name IBV_ABI_REQ(_enum);                               \
	typedef struct _kabi IBV_KABI_REQ(_enum);                              \
	typedef struct _kabi_resp IBV_KABI_RESP(_enum);                        \
	enum { IBV_ABI_ALIGN(_enum) = 8 };                                     \

#define DECLARE_CMD_EX(_enum, _name, _kabi)                                    \
	DECLARE_CMD_EXX(_enum, _name, _kabi, _kabi##_resp)

  /*
   * Define the ABI struct for use by the driver. The internal cmd APIs require
   * this layout. The driver specifies the enum # they wish to define for and
   * the base name, and the macros figure out the rest correctly.
   *
   * The static asserts check that the layout produced by the wrapper struct has
   * no implicit padding in strange places, specifically between the core
   * structure and the driver structure and between the driver structure and the
   * end of the struct.
   *
   * Implicit padding can arise in various cases where the structs are not sizes
   * to a multiple of 8 bytes.
   */
#define DECLARE_DRV_CMD(_name, _enum, _kabi_req, _kabi_resp)                   \
	struct _name {                                                         \
		IBV_ABI_REQ(_enum) ibv_cmd;                                    \
		union {                                                       \
                  struct _kabi_req;                                    \
                  struct _kabi_req drv_payload;                          \
              };                                                          \
	};                                                                     \
	struct _name##_resp {                                                  \
		IBV_KABI_RESP(_enum) ibv_resp;                                 \
		union {                                                       \
                  struct _kabi_resp;                                    \
   		    struct _kabi_resp drv_payload;                         \
		 };                                                          \
	};                                                                     \
/* Drivers may use 'empty' for _kabi to signal no struct */
struct empty { uint8_t dummy; };
#define _STRUCT_empty struct { uint8_t dummy; }

DECLARE_CMD(IB_USER_VERBS_CMD_GET_CONTEXT, ibv_get_context, ib_uverbs_get_context);

DECLARE_CMD_EX(IB_USER_VERBS_EX_CMD_QUERY_DEVICE, ibv_query_device_ex, ib_uverbs_ex_query_device);
DECLARE_CMD(IB_USER_VERBS_CMD_ALLOC_PD, ibv_alloc_pd, ib_uverbs_alloc_pd);
DECLARE_DRV_CMD(efa_alloc_pd, IB_USER_VERBS_CMD_ALLOC_PD, empty,
efa_ibv_alloc_pd_resp);
DECLARE_CMD(IB_USER_VERBS_CMD_CREATE_CQ, ibv_create_cq, ib_uverbs_create_cq);
DECLARE_CMD_EX(IB_USER_VERBS_EX_CMD_CREATE_CQ, ibv_create_cq_ex, ib_uverbs_ex_create_cq);
DECLARE_DRV_CMD(efa_create_cq, IB_USER_VERBS_EX_CMD_CREATE_CQ,
efa_ibv_create_cq, efa_ibv_create_cq_resp);
DECLARE_CMD(IB_USER_VERBS_CMD_CREATE_QP, ibv_create_qp, ib_uverbs_create_qp);
DECLARE_DRV_CMD(efa_create_qp, IB_USER_VERBS_CMD_CREATE_QP, efa_ibv_create_qp,
efa_ibv_create_qp_resp);
DECLARE_CMD(IB_USER_VERBS_CMD_CREATE_AH, ibv_create_ah, ib_uverbs_create_ah);
DECLARE_DRV_CMD(efa_create_ah, IB_USER_VERBS_CMD_CREATE_AH, empty,
efa_ibv_create_ah_resp);
DECLARE_CMD(IB_USER_VERBS_CMD_QUERY_DEVICE, ibv_query_device, ib_uverbs_query_device);
DECLARE_DRV_CMD(efa_query_device_ex, IB_USER_VERBS_EX_CMD_QUERY_DEVICE, empty,
efa_ibv_ex_query_device_resp);
DECLARE_CMD(IB_USER_VERBS_CMD_QUERY_PORT, ibv_query_port, ib_uverbs_query_port);
DECLARE_CMDX(IB_USER_VERBS_CMD_MODIFY_QP, ibv_modify_qp, ib_uverbs_modify_qp, empty);
DECLARE_CMD(IB_USER_VERBS_CMD_REG_MR, ibv_reg_mr, ib_uverbs_reg_mr);
DECLARE_CMD(IB_USER_VERBS_CMD_QUERY_QP, ibv_query_qp, ib_uverbs_query_qp);
DECLARE_DRV_CMD(efa_alloc_ucontext, IB_USER_VERBS_CMD_GET_CONTEXT,
    efa_ibv_alloc_ucontext_cmd, efa_ibv_alloc_ucontext_resp);
#pragma pack(pop)

#endif