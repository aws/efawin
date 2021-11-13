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

#include "efa_win_cmd.h"
#include "efa_io_defs.h" /* entry sizes */
#include "util/util.h"

static struct efa_context** ctx_list = NULL;
static int dev_cnt = 0;

int efa_device_close(struct efa_context* ctx)
{
    int err;
    struct efa_dev* edev = to_efa_dev(ctx->ibvctx.context.device);

    err = efa_close_device(&edev->device);
    efa_free_context(&ctx->ibvctx.context);

    return err;
}

struct efa_context* efa_device_open(struct ibv_device* device)
{
    struct efa_context* ctx;
    struct efa_dev* edev = to_efa_dev(device);
    EFA_QUERY_DEVICE_PARAMS params = { 0 };
    int err;

    params.InterfaceVersion = EFA_API_INTERFACE_VERSION;
    struct verbs_context* verbs_ctx;

    err = efa_open_device(&edev->device);
    if (err)
        goto err_return;

    // Because these attributes do not change often, we cache them in our device
    // structure so we only have to query the device once.
    err = efa_win_get_device_info(&edev->device, &params, &edev->dev_attr);
    if (err)
        goto err_close_device;

    verbs_ctx = efa_alloc_context(device, 0, NULL);

    if (!verbs_ctx)
        goto err_close_device;

    ctx = container_of(verbs_ctx, struct efa_context, ibvctx);

    ctx->ibvctx.context.device = device;
    ctx->sub_cqs_per_cq = edev->dev_attr.NumSubCqs;
    ctx->max_mr_size = edev->dev_attr.MaxMr;
    ctx->inline_buf_size = edev->dev_attr.InlineBufSize;
    ctx->max_llq_size = edev->dev_attr.MaxLlqSize;
    ctx->cqe_size = sizeof(struct efa_io_rx_cdesc);
    edev->pg_sz = PAGE_SIZE;
    pthread_mutex_init(&ctx->ibvctx.context.mutex, 0);
    return ctx;

err_close_device:
    efa_close_device(&edev->device);
err_return:
    return NULL;
}

void ibv_free_device_list(struct ibv_device** list)
{
    int i;
    for (i = 0; i < dev_cnt; i++)
        efa_device_close(ctx_list[i]);
    free(list);
}

struct ibv_device** ibv_get_device_list(int* num_devices)
{
    struct ibv_device** deviceptr;

    deviceptr = calloc(1, sizeof(struct ibv_device*));
    if (!deviceptr)
    {
        return NULL;
    }
    *deviceptr = calloc(1, sizeof(struct efa_dev));
    if (!*deviceptr)
    {
        free(deviceptr);
        return NULL;
    }
    strcpy_s((*deviceptr)->dev_name, IBV_SYSFS_NAME_MAX, "EFA");
    strcpy_s((*deviceptr)->name, IBV_SYSFS_NAME_MAX, "EFA");
    *num_devices = 1;

    return deviceptr;
}

int efa_device_init(void)
{
    struct ibv_device** device_list = NULL;
    int ctx_idx;
    int ret;

    device_list = ibv_get_device_list(&dev_cnt);

    ctx_list = calloc(dev_cnt, sizeof(*ctx_list));
    if (!ctx_list) {
        ret = -ENOMEM;
        goto err_free_dev_list;
    }

    for (ctx_idx = 0; ctx_idx < dev_cnt; ctx_idx++) {
        ctx_list[ctx_idx] = efa_device_open(device_list[ctx_idx]);
        if (!ctx_list[ctx_idx]) {
            ret = -ENODEV;
            goto err_close_devs;
        }
    }

    free(device_list);

    return 0;

err_close_devs:
    for (ctx_idx--; ctx_idx >= 0; ctx_idx--)
        efa_device_close(ctx_list[ctx_idx]);
    free(ctx_list);
    ctx_list = NULL;
err_free_dev_list:
    free(device_list);
    dev_cnt = 0;
    return ret;
}

void efa_device_free(void)
{
    int i;

    for (i = 0; i < dev_cnt; i++)
        efa_device_close(ctx_list[i]);

    free(ctx_list);
    ctx_list = NULL;
    dev_cnt = 0;
}

struct efa_context** efa_device_get_context_list(int* num_ctx)
{
    struct efa_context** devs = NULL;
    int i;

    devs = calloc(dev_cnt, sizeof(*devs));
    if (!devs)
        goto out;

    for (i = 0; i < dev_cnt; i++)
        devs[i] = ctx_list[i];
out:
    *num_ctx = devs ? dev_cnt : 0;
    return devs;
}

void efa_device_free_context_list(struct efa_context** list)
{
    free(list);
}

static int efa_everbs_cmd_get_ex_query_dev(struct efa_context* ctx,
    struct ibv_device_attr* attr)
{
    char* fw_ver;
    struct efa_dev* edev = to_efa_dev(ctx->ibvctx.context.device);
    attr->max_ah = edev->dev_attr.MaxAh;
    ZeroMemory(attr, sizeof(struct ibv_device_attr));

    // We query device attributes once in efa_device_open,
    // and cache them in the efa_device struct.
    // This function merely reads the cached values.

    fw_ver = (char*)&edev->dev_attr.FirmwareVersion;
    snprintf(attr->fw_ver, sizeof(edev->dev_attr.FirmwareVersion), "%u.%u.%u.%u",
        fw_ver[0], fw_ver[1], fw_ver[2], fw_ver[3]);
    fw_ver[sizeof(edev->dev_attr.FirmwareVersion)] = '\0';
    attr->hw_ver = edev->dev_attr.DeviceVersion;
    attr->vendor_part_id = 0xefa0;
    attr->vendor_id = 0x1d0f;

    attr->max_mr_size = edev->dev_attr.MaxMrPages * PAGE_SIZE;
    attr->page_size_cap = edev->dev_attr.PageSizeCap;
    attr->max_qp = edev->dev_attr.MaxQp;
    attr->max_cq = edev->dev_attr.MaxCq;
    attr->max_pd = edev->dev_attr.MaxPd;
    attr->max_mr = edev->dev_attr.MaxMr;
    attr->max_ah = edev->dev_attr.MaxAh;
    attr->max_cqe = edev->dev_attr.MaxCqDepth;
    attr->max_qp_wr = min(edev->dev_attr.MaxSqDepth,
        edev->dev_attr.MaxRqDepth);
    attr->max_sge = min(edev->dev_attr.MaxSqSge,
        edev->dev_attr.MaxRqSge);
    attr->page_size_cap = edev->dev_attr.PageSizeCap;
    ctx->inline_buf_size = edev->dev_attr.InlineBufSize;
    ctx->device_caps = edev->dev_attr.SupportedFeatures;
    ctx->max_llq_size = edev->dev_attr.MaxLlqSize;
    ctx->sub_cqs_per_cq = edev->dev_attr.NumSubCqs;
    ctx->max_mr_size = edev->dev_attr.MaxMr;
    ctx->max_rq_sge = edev->dev_attr.MaxRqSge;
    ctx->max_sq_sge = edev->dev_attr.MaxSqSge;
    ctx->max_rq_wr = edev->dev_attr.MaxRqDepth;
    ctx->max_sq_wr = edev->dev_attr.MaxSqDepth;
    ctx->max_tx_batch = 1;  // max_tx_batch retrieval is not supported on current Windows Driver.
    return 0;
}


int efa_cmd_query_device(struct efa_context* ctx, struct ibv_device_attr* attr)
{
    // Normally this function calls one of three query_device functions based on some conditions,
    // but on windows we will only ever call this one.
    return efa_everbs_cmd_get_ex_query_dev(ctx, attr);
}

int efa_cmd_query_port(struct efa_context* ctx, uint8_t port, struct ibv_port_attr* attr)
{
    // We cache these attribute in the efa_device on efa_device_open.
    // Read values here, instead of querying device.
    struct efa_dev* edev = to_efa_dev(ctx->ibvctx.context.device);
    attr->lmc = 1;
    attr->state = IBV_PORT_ACTIVE;
    attr->gid_tbl_len = 1;
    attr->pkey_tbl_len = 1;
    attr->max_mtu = edev->dev_attr.Mtu;
    attr->active_mtu = edev->dev_attr.Mtu;
    attr->max_msg_sz = edev->dev_attr.Mtu;
    attr->max_vl_num = 1;

    return 0;
}

int efa_cmd_query_gid(struct efa_context* ctx, uint8_t port_num,
    int index, union ibv_gid* gid)
{
    struct efa_dev* edev = to_efa_dev(ctx->ibvctx.context.device);
    memcpy(&gid->raw, edev->dev_attr.Addr, EFA_GID_SIZE);  // Use cached address as gid

    return 0;
}
