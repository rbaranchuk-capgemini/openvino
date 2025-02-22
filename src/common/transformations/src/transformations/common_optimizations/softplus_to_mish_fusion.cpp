// Copyright (C) 2018-2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "transformations/common_optimizations/softplus_to_mish_fusion.hpp"

#include <memory>
#include <ngraph/pattern/op/wrap_type.hpp>
#include <ngraph/rt_info.hpp>
#include <openvino/opsets/opset4.hpp>
#include <vector>

#include "itt.hpp"

ov::pass::SoftPlusToMishFusion::SoftPlusToMishFusion() {
    MATCHER_SCOPE(SoftPlusToMishFusion);
    auto input = ngraph::pattern::any_input();
    auto softplus = ngraph::pattern::wrap_type<opset4::SoftPlus>({input}, pattern::consumers_count(1));
    auto tanh = ngraph::pattern::wrap_type<opset4::Tanh>({softplus}, pattern::consumers_count(1));
    auto mul = std::make_shared<opset4::Multiply>(input, tanh);

    ov::matcher_pass_callback callback = [=](ngraph::pattern::Matcher& m) {
        auto& pattern_to_output = m.get_pattern_value_map();
        auto exp_input = pattern_to_output.at(input);

        auto mish = std::make_shared<opset4::Mish>(exp_input);

        mish->set_friendly_name(m.get_match_root()->get_friendly_name());
        ngraph::copy_runtime_info({pattern_to_output.at(mul).get_node_shared_ptr(),
                                   pattern_to_output.at(tanh).get_node_shared_ptr(),
                                   pattern_to_output.at(softplus).get_node_shared_ptr()},
                                  mish);
        ngraph::replace_node(m.get_match_root(), mish);
        return true;
    };

    auto m = std::make_shared<ngraph::pattern::Matcher>(mul, matcher_name);
    register_matcher(m, callback);
}
