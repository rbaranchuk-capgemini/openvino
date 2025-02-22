// Copyright (C) 2018-2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <memory>
#include <ngraph/ngraph.hpp>
#include <openvino/pass/graph_rewrite.hpp>
#include <transformations_visibility.hpp>
#include <vector>

#include "ngraph/pattern/matcher.hpp"

namespace ov {
namespace pass {

class TRANSFORMATIONS_API NormalizeL2Decomposition;

}  // namespace pass
}  // namespace ov

/**
 * @ingroup ie_transformation_common_api
 * @brief Decomposes NormalizeL2 into subgraph
 */
class ov::pass::NormalizeL2Decomposition : public ov::pass::MatcherPass {
public:
    OPENVINO_RTTI("NormalizeL2Decomposition", "0");
    NormalizeL2Decomposition();
};

namespace ngraph {
namespace pass {
using ov::pass::NormalizeL2Decomposition;
}  // namespace pass
}  // namespace ngraph
