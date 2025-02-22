// Copyright (C) 2018-2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
#include "behavior/ov_executable_network/exec_graph_info.hpp"
#include "ov_api_conformance_helpers.hpp"

#include "ie_plugin_config.hpp"
#include <common_test_utils/test_constants.hpp>


using namespace ov::test::behavior;
using namespace ov::test::conformance;
namespace {

const std::vector<ov::element::Type_t> ovExecGraphInfoElemTypes = {
        ov::element::i8,
        ov::element::i16,
        ov::element::i32,
        ov::element::i64,
        ov::element::u8,
        ov::element::u16,
        ov::element::u32,
        ov::element::u64,
        ov::element::f16,
        ov::element::f32,
        ov::element::f64,
        ov::element::bf16,
        ov::element::boolean,
};

INSTANTIATE_TEST_SUITE_P(ov_compiled_model,
                         OVExecGraphImportExportTest,
                         ::testing::Combine(
                                 ::testing::ValuesIn(ovExecGraphInfoElemTypes),
                                 ::testing::ValuesIn(return_all_possible_device_combination()),
                                 ::testing::Values(ov_config)),
                         OVExecGraphImportExportTest::getTestCaseName);
}  // namespace