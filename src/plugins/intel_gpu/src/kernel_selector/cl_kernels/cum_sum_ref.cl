// Copyright (C) 2018-2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "include/batch_headers/data_types.cl"
#include "include/batch_headers/fetch_data.cl"

///////////////////////// Input Index /////////////////////////
inline uint FUNC(get_input_index)(OPTIONAL_SHAPE_INFO_ARG uint b, uint f, uint w, uint z, uint y, uint x)
{
#if INPUT0_DIMS < 5
    return INPUT0_GET_INDEX(b, f, y, x);
#elif INPUT0_DIMS == 5
    return INPUT0_GET_INDEX(b, f, z, y, x);
#elif INPUT0_DIMS == 6
    return INPUT0_GET_INDEX(b, f, w, z, y, x);
#else
#error cum_sum_ref.cl: input format - not supported
#endif
}

///////////////////////// Output Index /////////////////////////
inline uint FUNC(get_output_index)(OPTIONAL_SHAPE_INFO_ARG uint b, uint f, uint w, uint z, uint y, uint x)
{
#if OUTPUT_DIMS < 5
    return OUTPUT_GET_INDEX(b, f, y, x);
#elif OUTPUT_DIMS == 5
    return OUTPUT_GET_INDEX(b, f, z, y, x);
#elif OUTPUT_DIMS == 6
    return OUTPUT_GET_INDEX(b, f, w, z, y, x);
#else
#error cum_sum_ref.cl: output format - not supported
#endif
}

KERNEL(cum_sum_ref)(
    OPTIONAL_SHAPE_INFO_ARG
    const __global INPUT0_TYPE* input,
    __global OUTPUT_TYPE* output)
{
    const uint batch = (uint)get_global_id(0);
    const uint features = (uint)get_global_id(1) / OUTPUT_SIZE_W;
    const uint w = (uint)get_global_id(1) % OUTPUT_SIZE_W;
    const uint yx = (uint)get_global_id(2) % (OUTPUT_SIZE_X * OUTPUT_SIZE_Y);
    const uint z = (uint)get_global_id(2) / (OUTPUT_SIZE_X * OUTPUT_SIZE_Y);
    const uint y = yx / OUTPUT_SIZE_X;
    const uint x = yx % OUTPUT_SIZE_X;

    int axes[6];
    axes[0] = batch;
    axes[1] = features;
    axes[2] = w;
    axes[3] = z;
    axes[4] = y;
    axes[5] = x;

    int stop_ind = axes[AXIS] + 1;

#ifdef REVERSE
    stop_ind = OUTPUT_SIZES[AXIS_LAYOUT_INDEX];
#ifdef EXCLUSIVE
    ++axes[AXIS];
#endif
#else
    axes[AXIS] = 0;
#ifdef EXCLUSIVE
    --stop_ind;
#endif
#endif

    INPUT0_TYPE res = INPUT0_VAL_ZERO;
    for (; axes[AXIS] < stop_ind; ++axes[AXIS]) {
        uint ind = FUNC_CALL(get_input_index)(OPTIONAL_SHAPE_INFO_TENSOR axes[0], axes[1], axes[2], axes[3], axes[4], axes[5]);
        res += input[ind];
    }

    uint out_ind = FUNC_CALL(get_output_index)(OPTIONAL_SHAPE_INFO_TENSOR batch, features, w, z, y, x);
    output[out_ind] = ACTIVATION(TO_OUTPUT_TYPE(res), ACTIVATION_PARAMS);
}
