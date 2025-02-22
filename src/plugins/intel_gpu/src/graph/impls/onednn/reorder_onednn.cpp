// Copyright (C) 2018-2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "reorder_inst.h"
#include "primitive_onednn_base.h"
#include "impls/implementation_map.hpp"

#include "kernel_selector_common.h"

#include <oneapi/dnnl/dnnl.hpp>

#include <algorithm>
#include <memory>
namespace cldnn {
namespace onednn {

struct reorder_onednn : typed_primitive_onednn_impl<reorder, void, dnnl::reorder::primitive_desc, dnnl::reorder> {
    using parent = typed_primitive_onednn_impl<reorder, void, dnnl::reorder::primitive_desc, dnnl::reorder>;
    using parent::parent;

    DECLARE_OBJECT_TYPE_SERIALIZATION

protected:
    std::unique_ptr<primitive_impl> clone() const override {
        return make_unique<reorder_onednn>(*this);
    }

    std::unordered_map<int, dnnl::memory> get_arguments(reorder_inst& instance) const override {
        std::unordered_map<int, dnnl::memory> args;

        int input_idx = DNNL_ARG_FROM;
        for (size_t i = 0; i < instance.inputs_memory_count(); i++) {
            auto& input = instance.input_memory(i);
            args.insert({input_idx++, input.get_onednn_memory(_pd.src_desc())});
        }

        {
            auto& output = instance.output_memory();
            args.insert({DNNL_ARG_TO, output.get_onednn_memory(_pd.dst_desc())});
        }

        return args;
    }

    static std::shared_ptr<dnnl::reorder::primitive_desc> get_reorder_descriptor(const kernel_impl_params& impl_params,
                                                                                 const dnnl::primitive_attr& attr,
                                                                                 const cldnn::engine& engine) {
        auto prim = impl_params.typed_desc<reorder>();

        auto input_layout = impl_params.get_input_layout(0);
        auto output_layout = impl_params.get_output_layout();

        auto input_md = onednn::layout_to_memory_desc(input_layout);
        auto output_md = onednn::layout_to_memory_desc(output_layout);

        return std::make_shared<dnnl::reorder::primitive_desc>(
            engine.get_onednn_engine(),
            input_md,
            engine.get_onednn_engine(),
            output_md,
            attr);
    }

public:
    void save(BinaryOutputBuffer& ob) const override {
        parent::save(ob);

        std::vector<uint8_t> prim_cache;
        prim_cache = _prim.get_cache_blob();
        ob << prim_cache;
    }

    void load(BinaryInputBuffer& ib) override {
        parent::load(ib);

        const kernel_impl_params* impl_params = reinterpret_cast<kernel_impl_params*>(ib.getKernlImplParams());
        auto desc = get_reorder_descriptor(*impl_params, *_attrs, ib.get_engine());
        _pd = *desc;

        std::vector<uint8_t> prim_cache;
        ib >> prim_cache;

        _prim = dnnl::reorder(_pd, prim_cache);
    }

    static std::unique_ptr<primitive_impl> create(const reorder_node& arg, const kernel_impl_params& impl_params) {
        auto& engine = impl_params.prog->get_engine();
        auto attr = arg.get_onednn_primitive_attributes();
        auto desc = get_reorder_descriptor(impl_params, *attr, impl_params.prog->get_engine());

        std::shared_ptr<void> dummy = nullptr;

        return cldnn::make_unique<reorder_onednn>(engine, dummy, attr, *desc);
    }
};

namespace detail {

attach_reorder_onednn::attach_reorder_onednn() {
    implementation_map<reorder>::add(impl_types::onednn, reorder_onednn::create, {});
}

}  // namespace detail
}  // namespace onednn
}  // namespace cldnn

BIND_BINARY_BUFFER_WITH_TYPE(cldnn::onednn::reorder_onednn)
