// Copyright (C) 2018-2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "intel_gpu/primitives/convolution.hpp"
#include "primitive_inst.h"

#include <memory>
#include <string>

namespace cldnn {

template <>
struct typed_program_node<deformable_conv> : public typed_program_node_base<deformable_conv> {
    using parent = typed_program_node_base<deformable_conv>;

public:
    typed_program_node(std::shared_ptr<primitive> prim, program& prog)
            : parent(prim, prog),
              split(this->get_primitive()->split()),
              depthwise_sep_opt(false),
              transposed(false),
              groups(this->get_primitive()->groups) {
        support_padding_all(true);
    }

    void set_split(int32_t node_split) { split = node_split; }
    int32_t get_split() const { return split; }

    void set_depthwise_sep_opt(bool node_depthwise_sep_opt) { depthwise_sep_opt = node_depthwise_sep_opt; }
    bool get_depthwise_sep_opt() const { return depthwise_sep_opt; }

    void set_transposed(bool node_transposed) { transposed = node_transposed; }
    bool get_transposed() const { return transposed; }

    void set_groups(uint32_t node_groups) { groups = node_groups; }
    uint32_t get_groups() const { return groups; }

    program_node& input() const { return get_dependency(0); }

    program_node& weights(size_t idx = 0) const {
        if (static_cast<int32_t>(idx) >= this->get_split())
            throw std::range_error("weights offset too big");

        return get_dependency(1 + idx);
    }

    program_node& bias(size_t idx = 0) const {
        if (static_cast<int32_t>(idx) >= this->get_split())
            throw std::range_error("bias offset too big");

        return get_dependency(1 + this->get_split() + idx);
    }

    bool bias_term() const { return get_primitive()->bias.size() > 0; }

    std::unique_ptr<kernel_impl_params> get_kernel_impl_params(const std::vector<layout>& in_layouts, const std::vector<layout>& out_layouts) const override {
        auto params = parent::get_kernel_impl_params(in_layouts, out_layouts);
        params->weights_layout = optional_layout(weights().get_output_layout());
        if (bias_term())
            params->bias_layout = optional_layout(bias().get_output_layout());
        return params;
    }

private:
    int32_t split;
    bool depthwise_sep_opt;
    bool transposed;
    uint32_t groups;
};

using deformable_conv_node = typed_program_node<deformable_conv>;

template <>
class typed_primitive_inst<deformable_conv> : public typed_primitive_inst_base<deformable_conv> {
    using parent = typed_primitive_inst_base<deformable_conv>;
    using parent::parent;

public:
    static layout calc_output_layout(deformable_conv_node const& node, kernel_impl_params const& impl_param);
    static std::string to_string(deformable_conv_node const& node);

public:
    typed_primitive_inst(network& network, deformable_conv_node const& node);

    memory::ptr weights_memory(size_t index) const {
        if (node->get_groups() == 1) {
            if (static_cast<int32_t>(index) >= node->get_split())
                throw std::range_error("weights offset too big");
            return dep_memory_ptr(1 + index);
        } else {  // all weights are in one buffer
            return dep_memory_ptr(1);
        }
    }

    memory::ptr bias_memory(size_t index) const {
        if (node->get_groups() == 1) {
            if (static_cast<int32_t>(index) >= node->get_split())
                throw std::range_error("bias offset too big");
            return dep_memory_ptr(1 + node->get_split());
        } else {  // all bias are in one buffer
            return dep_memory_ptr(2);
        }
    }

    bool bias_term() const { return node->bias_term(); }
};

using deformable_conv_inst = typed_primitive_inst<deformable_conv>;

template <>
struct typed_program_node<deformable_interp> : public typed_program_node_base<deformable_interp> {
    using parent = typed_program_node_base<deformable_interp>;
    using parent::parent;

public:
    typed_program_node(std::shared_ptr<primitive> prim, program& prog)
            : parent(prim, prog),
              split(1),
              depthwise_sep_opt(false),
              transposed(false),
              groups(this->get_primitive()->groups),
              deformable_groups(this->get_primitive()->deformable_groups) {
        support_padding_all(true);
    }

    void set_split(int32_t node_split) { split = node_split; }
    int32_t get_split() const { return split; }

    void set_depthwise_sep_opt(bool node_depthwise_sep_opt) { depthwise_sep_opt = node_depthwise_sep_opt; }
    bool get_depthwise_sep_opt() const { return depthwise_sep_opt; }

    void set_transposed(bool node_transposed) { transposed = node_transposed; }
    bool get_transposed() const { return transposed; }

    void set_groups(uint32_t node_groups) { groups = node_groups; }
    uint32_t get_groups() const { return groups; }

    void set_deformable_groups(uint32_t node_deformable_groups) { deformable_groups = node_deformable_groups; }
    uint32_t get_deformable_groups() const { return deformable_groups; }

    program_node& input() const { return get_dependency(0); }
    program_node& trans() const { return get_dependency(1); }
    program_node& mask() const { return get_dependency(2); }

private:
    int32_t split;
    bool depthwise_sep_opt;
    bool transposed;
    uint32_t groups;
    uint32_t deformable_groups;
};

using deformable_interp_node = typed_program_node<deformable_interp>;

template <>
class typed_primitive_inst<deformable_interp> : public typed_primitive_inst_base<deformable_interp> {
    using parent = typed_primitive_inst_base<deformable_interp>;
    using parent::parent;

public:
    static layout calc_output_layout(deformable_interp_node const& node, kernel_impl_params const& impl_param);
    static std::string to_string(deformable_interp_node const& node);

public:
    typed_primitive_inst(network& network, deformable_interp_node const& node);

    memory& trans_memory() const { return dep_memory(1); }
    memory& mask_memory() const { return dep_memory(2); }
};

using deformable_interp_inst = typed_primitive_inst<deformable_interp>;

}  // namespace cldnn
