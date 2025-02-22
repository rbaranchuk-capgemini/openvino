// Copyright (C) 2018-2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <memory>
#include <openvino/pass/graph_rewrite.hpp>
#include <transformations_visibility.hpp>

namespace ov {
namespace pass {

class TRANSFORMATIONS_API SoftmaxDecomposition;

}  // namespace pass
}  // namespace ov

/**
 * @ingroup ie_transformation_common_api
 * @brief SoftmaxDecomposition transformation replaces softmax with following graph:
 *
 *            +---------------+
 *            │               │
 *            │     input     │
 *            │               │
 *            +---------------+
 *                │      │
 *                │      v
 *                │ +-----------+
 *                │ │           │
 *                │ │ ReduceMax │
 *                │ │           │
 *                │ +-----------+
 *                │      │
 *                │      │
 *                v      v
 *            +---------------+
 *            │               │
 *            │      Sub      │
 *            │               │
 *            +---------------+
 *                    |
 *                    |
 *                    v
 *            +---------------+
 *            │               │
 *            │      Exp      │
 *            │               │
 *            +---------------+
 *                │      │
 *                │      v
 *                │ +-----------+
 *                │ │           │
 *                │ │ ReduceSum │
 *                │ │           │
 *                │ +-----------+
 *                │      │
 *                │      │
 *                v      v
 *             +-------------+
 *             |             │
 *             |     Div     │
 *             │             │
 *             +-------------+
 *
 */

class ov::pass::SoftmaxDecomposition : public ov::pass::MatcherPass {
public:
    OPENVINO_RTTI("SoftmaxDecomposition", "0");
    SoftmaxDecomposition();
};

namespace ngraph {
namespace pass {
using ov::pass::SoftmaxDecomposition;
}  // namespace pass
}  // namespace ngraph
