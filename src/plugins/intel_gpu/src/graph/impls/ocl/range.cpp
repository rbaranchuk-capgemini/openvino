// Copyright (C) 2018-2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include <range_inst.h>
#include "primitive_base.hpp"
#include <impls/implementation_map.hpp>
#include <kernel_selector_helper.h>
#include <range/range_kernel_selector.h>
#include <range/range_kernel_ref.h>
#include <intel_gpu/runtime/error_handler.hpp>

namespace cldnn {
namespace ocl {

struct range_impl : typed_primitive_impl_ocl<range> {
    using parent = typed_primitive_impl_ocl<range>;
    using parent::parent;
    using kernel_selector_t = kernel_selector::range_kernel_selector;
    using kernel_params_t = std::pair<kernel_selector::range_params, kernel_selector::range_optional_params>;

    DECLARE_OBJECT_TYPE_SERIALIZATION

    std::unique_ptr<primitive_impl> clone() const override {
        return make_unique<range_impl>(*this);
    }

    static kernel_params_t get_kernel_params(const kernel_impl_params& impl_param) {
        const auto& primitive = impl_param.typed_desc<range>();
        auto params = get_default_params<kernel_selector::range_params>(impl_param);
        for (int i : {1, 2})
            params.inputs.push_back(convert_data_tensor(impl_param.get_input_layout(i)));
        auto optional_params = get_default_optional_params<kernel_selector::range_optional_params>(impl_param.get_program());

        return {params, optional_params};
    }
};

namespace detail {

attach_range_impl::attach_range_impl() {
    implementation_map<range>::add(
        impl_types::ocl,
        typed_primitive_impl_ocl<range>::create<range_impl>,
        {
            std::make_tuple(data_types::u8, format::bfyx),
            std::make_tuple(data_types::i8, format::bfyx),
            std::make_tuple(data_types::f16, format::bfyx),
            std::make_tuple(data_types::f32, format::bfyx),
            std::make_tuple(data_types::i32, format::bfyx),
            std::make_tuple(data_types::i64, format::bfyx),
        });
}

}  // namespace detail
}  // namespace ocl
}  // namespace cldnn

BIND_BINARY_BUFFER_WITH_TYPE(cldnn::ocl::range_impl)
