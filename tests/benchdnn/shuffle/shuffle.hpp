/*******************************************************************************
* Copyright 2018-2022 Intel Corporation
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

#ifndef SHUFFLE_HPP
#define SHUFFLE_HPP

#include <assert.h>
#include <limits.h>
#include <stdint.h>

#include <iostream>

#include "common.hpp"
#include "dnn_types.hpp"
#include "dnnl_common.hpp"
#include "dnnl_debug.hpp"
#include "dnnl_memory.hpp"
#include "utils/perf_report.hpp"
#include "utils/settings.hpp"

namespace shuffle {

struct settings_t : public base_settings_t {
    settings_t() = default;

    // ctor to save certain fields from resetting
    settings_t(const char *perf_template) : settings_t() {
        this->perf_template = perf_template;
    }

    prb_dims_t prb_dims;

    std::vector<dir_t> dir {FWD_D};
    std::vector<dnnl_data_type_t> dt {dnnl_f32};
    std::vector<std::string> tag {tag::abx};
    std::vector<int64_t> group {1};
    std::vector<int> axis {1};

    const char *perf_template_csv
            = "perf,%engine%,%impl%,%dir%,%dt%,%tag%,%group%,%axis%,%DESC%,%-"
              "time%,%0time%";

    void reset() { *this = settings_t(perf_template); }
};

struct prb_t : public prb_dims_t {
    prb_t(const prb_dims_t &prb_dims, dir_t dir, dnnl_data_type_t dt,
            const std::string &tag, int axis, int64_t group, const attr_t &attr)
        : prb_dims_t(prb_dims)
        , dir(dir)
        , dt(dt)
        , tag(tag)
        , axis(axis)
        , group(group)
        , attr(attr) {}
    ~prb_t() {}

    dir_t dir;
    dnnl_data_type_t dt;
    std::string tag;
    int axis;
    int64_t group;
    attr_t attr;
};
std::ostream &operator<<(std::ostream &s, const prb_t &prb);

struct perf_report_t : public base_perf_report_t {
    perf_report_t(const prb_t *prb, const char *perf_template)
        : base_perf_report_t(perf_template)
        , p_(prb)
        , tag_(normalize_tag(p_->tag, p_->ndims)) {}

    void dump_desc(std::ostream &s) const override {
        s << static_cast<const prb_dims_t &>(*p_);
    }

    void dump_desc_csv(std::ostream &s) const override { dump_desc(s); }

    const std::string *name() const override { return &p_->name; }
    const int *axis() const override { return &p_->axis; }
    const int64_t *group() const override { return &p_->group; }
    const dir_t *dir() const override { return &p_->dir; }
    const dnnl_data_type_t *dt() const override { return &p_->dt; }
    const std::string *tag() const override { return &tag_; }

private:
    const prb_t *p_;
    std::string tag_;
};

inline size_t data_off(const prb_t *prb, int64_t mb, int64_t c, int64_t d,
        int64_t h, int64_t w) {
    const auto &dims = prb->dims;
    return (((mb * dims[1] + c) * dims[2] + d) * dims[3] + h) * dims[4] + w;
}

void compute_ref(const prb_t *prb, const dnn_mem_t &src, dnn_mem_t &dst);
int doit(const prb_t *prb, res_t *res);
int bench(int argc, char **argv);

void check_known_skipped_case(const prb_t *prb, res_t *res);
int init_pd(dnnl_engine_t engine, const prb_t *prb, dnnl_primitive_desc_t &spd,
        res_t *res, dir_t dir, const_dnnl_primitive_desc_t hint);
int fill_src(const prb_t *prb, dnn_mem_t &mem_dt, dnn_mem_t &mem_fp);
} // namespace shuffle

#endif
