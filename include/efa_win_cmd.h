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

#ifndef _EFA_WIN_CMD_H_
#define _EFA_WIN_CMD_H_

#include "efa-win-abi.h"
#include "efa.h"

struct efa_ex_query_device_resp {
    struct ib_uverbs_ex_query_device_resp ibv_resp;
    struct efa_ibv_ex_query_device_resp efa_resp;
};

int efa_device_close(struct efa_context* ctx);
struct efa_context* efa_device_open(struct ibv_device* device);
int efa_cmd_query_device(struct efa_context* ctx, struct ibv_device_attr* attr);
int efa_cmd_query_port(struct efa_context* ctx, uint8_t port, struct ibv_port_attr* attr);
int efa_cmd_query_gid(struct efa_context* ctx, uint8_t port_num,
    int index, union ibv_gid* gid);


#endif /* _EFA_WIN_CMD_H_ */