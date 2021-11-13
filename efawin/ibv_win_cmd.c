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

#include <windows.h>
#include <inttypes.h>
#include <util/util.h>

#include "efa_win_cmd.h"

int ibv_cmd_get_context(struct verbs_context* context,
    struct ibv_get_context* cmd, size_t cmd_size,
    struct ib_uverbs_get_context_resp* resp, size_t resp_size)
{
    struct efa_alloc_ucontext_resp* efa_resp = container_of(resp, struct efa_alloc_ucontext_resp, ibv_resp);
    struct efa_dev* dev = to_efa_dev(context->context.device);
    efa_resp->cmds_supp_udata_mask |= EFA_USER_CMDS_SUPP_UDATA_CREATE_AH;
    efa_resp->sub_cqs_per_cq = dev->dev_attr.NumSubCqs;
    efa_resp->inline_buf_size = dev->dev_attr.InlineBufSize;
    efa_resp->max_llq_size = dev->dev_attr.MaxLlqSize;
    return 0;
}

int ibv_cmd_query_device_any(struct ibv_context* context,
    const struct ibv_query_device_ex_input* input,
    struct ibv_device_attr_ex* attr, size_t attr_size,
    struct ib_uverbs_ex_query_device_resp* resp,
    size_t* resp_size)
{
    struct efa_dev* dev = to_efa_dev(context->device);
    attr->orig_attr.max_qp = dev->dev_attr.MaxQp;
    attr->orig_attr.max_cq = dev->dev_attr.MaxCq;
    attr->orig_attr.page_size_cap = dev->dev_attr.PageSizeCap;
    attr->orig_attr.device_cap_flags = dev->dev_attr.SupportedFeatures;
    attr->orig_attr.hw_ver = dev->dev_attr.DeviceVersion;
    attr->orig_attr.max_ah = dev->dev_attr.MaxAh;
    attr->orig_attr.max_cqe = dev->dev_attr.MaxCqDepth;
    attr->orig_attr.max_mr = dev->dev_attr.MaxMr;
    attr->orig_attr.max_mr_size = dev->dev_attr.MaxMrPages;
    attr->orig_attr.max_pd = dev->dev_attr.MaxPd;
    return 0;
}

int ibv_cmd_query_port(struct ibv_context* context, uint8_t port_num,
    struct ibv_port_attr* port_attr,
    struct ibv_query_port* cmd, size_t cmd_size)
{
    struct efa_dev* dev = to_efa_dev(context->device);
    port_attr->max_msg_sz = dev->dev_attr.Mtu;
    port_attr->active_mtu = dev->dev_attr.Mtu;
    port_attr->active_speed = 32; // 25.0 Gbps
    port_attr->active_width = 2; // x4
    port_attr->phys_state = 5; // Link Up
    port_attr->state = IBV_PORT_ACTIVE;
    port_attr->max_vl_num = 1;
    port_attr->gid_tbl_len = 1;
    return 0;
}

int ibv_cmd_alloc_pd(struct ibv_context* context, struct ibv_pd* pd,
    struct ibv_alloc_pd* cmd, size_t cmd_size,
    struct ib_uverbs_alloc_pd_resp* resp, size_t resp_size)
{
    struct efa_pd* efa_pd = to_efa_pd(pd);
    EFA_PD_INFO result = { 0 };
    struct efa_dev* edev = to_efa_dev(context->device);
    int err;

    err = efa_win_alloc_pd(&edev->device, &result);
    if (err) {
        return -ENODEV;
    }

    pd->context = context;
    pd->handle = result.Pdn;
    pd->context = context;
    efa_pd->pdn = result.Pdn;

    return 0;
}
int ibv_cmd_dealloc_pd(struct ibv_pd* pd)
{
    int err;
    EFA_DEALLOC_PD_PARAMS params = { 0 };
    struct efa_dev* edev = to_efa_dev(pd->context->device);

    params.Pdn = pd->handle;

    err = efa_win_dealloc_pd(&edev->device, &params);

    return err;
}

int ibv_cmd_reg_mr(struct ibv_pd* pd, void* addr, size_t length,
    uint64_t hca_va, int access,
    struct verbs_mr* vmr, struct ibv_reg_mr* cmd,
    size_t cmd_size,
    struct ib_uverbs_reg_mr_resp* resp, size_t resp_size)
{
    (void)hca_va;

    struct ibv_mr* mr = &vmr->ibv_mr;
    struct efa_dev* edev = to_efa_dev(pd->context->device);
    EFA_REG_MR_PARAMS params = { 0 };
    EFA_MR_INFO result = { 0 };
    int err;

    params.MrAddr = addr;
    params.Pdn = pd->handle;
    params.MrLen = length;
    params.Permissions = access;

    err = efa_win_register_mr(&edev->device, &params, &result);
    if (!err) {
        mr->addr = addr;
        mr->context = pd->context;
        mr->handle = 0;
        mr->length = length;
        mr->lkey = result.LKey;
        mr->rkey = result.RKey;
        mr->pd = pd;
    }

    return err;
}

int ibv_cmd_dereg_mr(struct verbs_mr* vmr)
{
    int err;
    struct efa_dev* edev;
    EFA_DEREG_MR_PARAMS params = { 0 };

    if (!vmr->ibv_mr.context) {
        return -ENOBUFS;
    }

    params.LKey = vmr->ibv_mr.lkey;

    edev = to_efa_dev(vmr->ibv_mr.context->device);
    err = efa_win_dereg_mr(&edev->device, &params);

    return err;
}

int ibv_cmd_create_cq_ex(struct ibv_context* context,
    const struct ibv_cq_init_attr_ex* cq_attr,
    struct verbs_cq* cq,
    struct ibv_create_cq_ex* cmd,
    size_t cmd_size,
    struct ib_uverbs_ex_create_cq_resp* resp,
    size_t resp_size,
    uint32_t cmd_flags)
{
    struct ibv_cq* ibv_cq = NULL;
    struct efa_context* ctx = to_efa_context(context);
    struct efa_dev* edev = to_efa_dev(context->device);
    struct efa_cq* efa_cq = to_efa_cq(&cq->cq);
    struct efa_create_cq_resp* efa_resp = container_of(resp, struct efa_create_cq_resp, ibv_resp);
    EFA_CREATE_CQ_PARAMS params = { 0 };
    EFA_CQ_INFO result = { 0 };
    int err;
    uint64_t cqe_rounded;

    if (resp_size > UINT32_MAX) {
        return E_INVALIDARG;
    }

    cqe_rounded = roundup_pow_of_two(cq_attr->cqe);
    if (cqe_rounded > UINT16_MAX) {
        return E_INVALIDARG;
    }

    params.CqDepth = (UINT16)cqe_rounded;
    params.NumSubCqs = ctx->sub_cqs_per_cq;

    if (ctx->cqe_size > UINT8_MAX) {
        return E_INVALIDARG;
    }
    params.EntrySizeInBytes = (UINT8)ctx->cqe_size;

    err = efa_win_create_cq(&edev->device, &params, &result);
    if (err) {
        return err;
    }

    ibv_cq = &cq->cq;
    ibv_cq->async_events_completed = 0;
    ibv_cq->comp_events_completed = 0;
    ibv_cq->channel = cq_attr->channel;
    ibv_cq->context = context;
    ibv_cq->cqe = result.CqActualDepth;
    ibv_cq->cq_context = cq_attr->cq_context;
    ibv_cq->handle = result.CqIndex;
    pthread_cond_init(&ibv_cq->cond, 0);
    pthread_mutex_init(&ibv_cq->mutex, 0);

    efa_cq->buf = result.CqAddr;
    efa_cq->num_sub_cqs = ctx->sub_cqs_per_cq;
    efa_cq->buf_size = result.CqActualDepth * ctx->sub_cqs_per_cq * ctx->cqe_size;
    efa_cq->cqn = result.CqIndex;

    resp->base.cq_handle = result.CqIndex;
    resp->base.cqe = result.CqActualDepth;
    resp->response_length = (uint32_t)resp_size;
    efa_resp->q_mmap_size = efa_cq->buf_size;
    efa_resp->cq_idx = result.CqIndex;
    return 0;
}

int ibv_cmd_destroy_cq(struct ibv_cq* cq)
{
    int err;
    EFA_DESTROY_CQ_PARAMS params = { 0 };
    struct efa_dev* edev = to_efa_dev(cq->context->device);

    params.CqIndex = cq->handle;

    err = efa_win_destroy_cq(&edev->device, &params);

    pthread_cond_destroy(&cq->cond);
    pthread_mutex_destroy(&cq->mutex);

    return err;
}

int ibv_cmd_create_qp_ex(struct ibv_context* context,
    struct verbs_qp* qp,
    struct ibv_qp_init_attr_ex* attr_ex,
    struct ibv_create_qp* cmd, size_t cmd_size,
    struct ib_uverbs_create_qp_resp* resp, size_t resp_size)
{
    int err;
    EFA_CREATE_QP_PARAMS params = { 0 };
    EFA_QP_INFO result = { 0 };
    struct ibv_qp* ibv_qp = NULL;
    struct efa_context* ctx = to_efa_context(context);
    struct efa_dev* edev = to_efa_dev(attr_ex->pd->context->device);
    struct efa_qp* efa_qp = to_efa_qp(&qp->qp);
    struct efa_create_qp_resp* efa_resp = container_of(resp, struct efa_create_qp_resp, ibv_resp);

    params.Pdn = attr_ex->pd->handle;
    params.QpType = (attr_ex->qp_type == IBV_QPT_UD) ? EFA_WIN_QP_UD : EFA_WIN_QP_SRD;
    params.SendCqIndex = attr_ex->send_cq->handle;
    params.RecvCqIndex = attr_ex->recv_cq->handle;
    params.SqDepth = efa_qp->sq.wq.wqe_cnt;
    params.RqDepth = efa_qp->rq.wq.wqe_cnt;
    params.RqRingSizeInBytes = (efa_qp->rq.wq.desc_mask + 1) * sizeof(struct efa_io_rx_desc);;
    params.SqRingSizeInBytes = (efa_qp->sq.wq.desc_mask + 1) * sizeof(struct efa_io_tx_wqe);

    err = efa_win_create_qp(&edev->device, &params, &result);
    if (err) {
        return err;
    }

    ibv_qp = &qp->qp;
    ibv_qp->handle = result.QpHandle;
    ibv_qp->qp_num = result.QpNum;
    ibv_qp->context = attr_ex->pd->context;
    ibv_qp->qp_context = attr_ex->qp_context;
    ibv_qp->pd = attr_ex->pd;
    ibv_qp->send_cq = attr_ex->send_cq;
    ibv_qp->recv_cq = attr_ex->recv_cq;
    ibv_qp->qp_type = params.QpType;
    ibv_qp->state = IBV_QPS_RESET;
    ibv_qp->events_completed = 0;
    pthread_cond_init(&ibv_qp->cond, 0);
    pthread_mutex_init(&ibv_qp->mutex, 0);

    efa_qp->page_size = PAGE_SIZE;
    efa_qp->rq.buf_size = params.RqRingSizeInBytes;
    efa_qp->rq.buf = result.RqAddr;
    efa_qp->rq.wq.db = result.RqDoorbellAddr;
    efa_qp->rq.wq.sub_cq_idx = result.RecvSubCqIndex;
    efa_qp->sq.desc_ring_mmap_size = params.SqRingSizeInBytes;
    efa_qp->sq.desc = result.SqAddr;
    efa_qp->sq.wq.db = result.SqDoorbellAddr;
    efa_qp->sq.wq.sub_cq_idx = result.SendSubCqIndex;

    resp->qpn = result.QpNum;
    resp->max_inline_data = attr_ex->cap.max_inline_data;
    resp->max_recv_sge = attr_ex->cap.max_recv_sge;
    resp->max_send_sge = attr_ex->cap.max_send_sge;
    resp->max_recv_wr = attr_ex->cap.max_recv_wr;
    resp->max_send_wr = attr_ex->cap.max_send_wr;
    efa_resp->rq_mmap_size = efa_qp->rq.buf_size;
    efa_resp->recv_sub_cq_idx = result.RecvSubCqIndex;
    efa_resp->send_sub_cq_idx = result.SendSubCqIndex;
    return 0;
}

int ibv_cmd_query_qp(struct ibv_qp* qp, struct ibv_qp_attr* qp_attr,
    int attr_mask,
    struct ibv_qp_init_attr* qp_init_attr,
    struct ibv_query_qp* cmd, size_t cmd_size)
{
    return -1;
}

int ibv_cmd_modify_qp(struct ibv_qp* qp, struct ibv_qp_attr* attr,
    int attr_mask,
    struct ibv_modify_qp* cmd, size_t cmd_size)
{
    int err = 0;
    EFA_MODIFY_QP_PARAMS params = { 0 };
    EFA_QP_INFO result = { 0 };
    struct ibv_qp* ibv_qp = NULL;
    struct efa_dev* edev = to_efa_dev(qp->context->device);

    params.QpHandle = qp->handle;
    if (attr_mask & IBV_QP_STATE) {
        params.ModifyMask |= EFA_MODIFY_QP_STATE_MASK;
        params.QpState = attr->qp_state;
    }
    if (attr_mask & IBV_QP_CUR_STATE) {
        params.ModifyMask |= EFA_MODIFY_QP_CUR_STATE_MASK;
        params.QpCurState = attr->cur_qp_state;
    }
    if (attr_mask & IBV_QP_QKEY) {
        params.ModifyMask |= EFA_MODIFY_QP_QKEY_MASK;
        params.QKey = attr->qkey;
    }
    if (attr_mask & IBV_QP_SQ_PSN) {
        params.ModifyMask |= EFA_MODIFY_QP_SQ_PSN_MASK;
        params.SqPsn = attr->sq_psn;
    }
    if (attr_mask & IBV_QP_EN_SQD_ASYNC_NOTIFY) {
        params.ModifyMask |= EFA_MODIFY_QP_SQ_DRAINED_ASYNC_NOTIFY_MASK;
        params.SqDrainedAsyncNotify = attr->en_sqd_async_notify;
    }
    if (attr_mask & IBV_QP_RNR_RETRY) {
        params.ModifyMask |= EFA_MODIFY_QP_RNR_RETRY_MASK;
        params.RnrRetry = attr->rnr_retry;
    }

    err = efa_win_modify_qp(&edev->device, &params);
    return err;
}

int ibv_cmd_modify_qp_ex(struct ibv_qp* qp, struct ibv_qp_attr* attr,
    int attr_mask, struct ibv_modify_qp_ex* cmd,
    size_t cmd_size,
    struct ib_uverbs_ex_modify_qp_resp* resp,
    size_t resp_size)
{
    return ENOTSUP;
}

int ibv_cmd_destroy_qp(struct ibv_qp* qp)
{
    int err;
    EFA_DESTROY_QP_PARAMS params = { 0 };
    struct efa_dev* edev = to_efa_dev(qp->context->device);

    params.QpHandle = qp->handle;

    err = efa_win_destroy_qp(&edev->device, &params);

    pthread_cond_destroy(&qp->cond);
    pthread_mutex_destroy(&qp->mutex);

    return err;
}

int ibv_cmd_create_ah(struct ibv_pd* pd, struct ibv_ah* ah,
    struct ibv_ah_attr* attr,
    struct ib_uverbs_create_ah_resp* resp,
    size_t resp_size)
{
    int err = 0;
    struct efa_ah* efa_ah = to_efa_ah(ah);
    EFA_CREATE_AH_PARAMS params = { 0 };
    EFA_AH_INFO result = { 0 };
    struct efa_dev* edev = to_efa_dev(pd->context->device);
    struct efa_create_ah_resp* efa_resp = container_of(resp, struct efa_create_ah_resp, ibv_resp);

    params.Pdn = pd->handle;
    memcpy(&params.DestAddr, &attr->grh.dgid, ARRAYSIZE(params.DestAddr));

    err = efa_win_create_ah(&edev->device, &params, &result);
    if (err) {
        goto err_free_ah;
    }

    ah->context = pd->context;
    ah->handle = result.AddressHandle;
    ah->pd = pd;
    efa_ah->efa_ah = result.AddressHandle;
    resp->ah_handle = result.AddressHandle;
    efa_resp->efa_address_handle = result.AddressHandle;

    return err;

err_free_ah:
    free(ah);
    return err;
}
int ibv_cmd_destroy_ah(struct ibv_ah* ah)
{
    int err;
    EFA_DESTROY_AH_PARAMS params = { 0 };
    struct efa_dev* edev = to_efa_dev(ah->context->device);

    params.Pdn = ah->pd->handle;
    params.AddressHandle = ah->handle;

    err = efa_win_destroy_ah(&edev->device, &params);

    return err;
}
