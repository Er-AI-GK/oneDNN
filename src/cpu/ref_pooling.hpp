/*******************************************************************************
* Copyright 2016-2023 Intel Corporation
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/

#ifndef CPU_REF_POOLING_HPP
#define CPU_REF_POOLING_HPP

#include <assert.h>

#include "common/c_types_map.hpp"
#include "common/primitive.hpp"
#include "common/type_helpers.hpp"
#include "common/utils.hpp"

#include "cpu/platform.hpp"
#include "cpu/primitive_attr_postops.hpp"

#include "cpu/cpu_pooling_pd.hpp"

namespace dnnl {
namespace impl {
namespace cpu {

template <impl::data_type_t data_type, impl::data_type_t acc_type = data_type>
struct ref_pooling_fwd_t : public primitive_t {
    struct pd_t : public cpu_pooling_fwd_pd_t {
        using cpu_pooling_fwd_pd_t::cpu_pooling_fwd_pd_t;

        DECLARE_COMMON_PD_T("ref:any", ref_pooling_fwd_t);

        status_t init(engine_t *engine) {
            using sm = primitive_attr_t::skip_mask_t;

            bool ok = platform::has_data_type_support(data_type)
                    && set_default_params() == status::success && is_fwd()
                    && utils::everyone_is(
                            data_type, src_md()->data_type, dst_md()->data_type)
                    && desc()->accum_data_type == acc_type
                    && attr()->has_default_values(sm::post_ops)
                    && attr_.set_default_formats(dst_md(0)) == status::success;
            if (!ok) return status::unimplemented;

            bool is_training = desc_.prop_kind == prop_kind::forward_training;
            if (desc()->alg_kind == alg_kind::pooling_max && is_training)
                init_default_ws();

            return status::success;
        }
    };

    ref_pooling_fwd_t(const pd_t *apd) : primitive_t(apd) {}

    status_t init(engine_t *engine) override {
        ref_post_ops
                = utils::make_unique<ref_post_ops_t>(pd()->attr()->post_ops_);
        if (!ref_post_ops) return status::out_of_memory;
        return status::success;
    }

    using data_t = typename prec_traits<data_type>::type;
    using acc_data_t = typename prec_traits<acc_type>::type;

    status_t execute(const exec_ctx_t &ctx) const override {
        return execute_forward(ctx);
    }

private:
    const pd_t *pd() const { return (const pd_t *)primitive_t::pd().get(); }
    status_t execute_forward(const exec_ctx_t &ctx) const;
    std::unique_ptr<ref_post_ops_t> ref_post_ops;
};

template <impl::data_type_t data_type>
struct ref_pooling_bwd_t : public primitive_t {
    struct pd_t : public cpu_pooling_bwd_pd_t {
        using cpu_pooling_bwd_pd_t::cpu_pooling_bwd_pd_t;

        DECLARE_COMMON_PD_T("ref:any", ref_pooling_bwd_t);

        status_t init(engine_t *engine) {
            bool ok = platform::has_data_type_support(data_type)
                    && set_default_params() == status::success && !is_fwd()
                    && utils::everyone_is(data_type, diff_dst_md()->data_type,
                            diff_src_md()->data_type)
                    && attr()->has_default_values();
            if (!ok) return status::unimplemented;

            if (desc()->alg_kind == alg_kind::pooling_max) {
                const auto ws_dt = hint_fwd_pd_->workspace_md()->data_type;
                init_default_ws(ws_dt);
                if (!compare_ws(hint_fwd_pd_)) return status::unimplemented;
            }

            return status::success;
        }
    };

    ref_pooling_bwd_t(const pd_t *apd) : primitive_t(apd) {}
    typedef typename prec_traits<data_type>::type data_t;

    status_t execute(const exec_ctx_t &ctx) const override {
        return execute_backward(ctx);
    }

private:
    status_t execute_backward(const exec_ctx_t &ctx) const;
    const pd_t *pd() const { return (const pd_t *)primitive_t::pd().get(); }
};

} // namespace cpu
} // namespace impl
} // namespace dnnl

#endif

// vim: et ts=4 sw=4 cindent cino+=l0,\:4,N-s
