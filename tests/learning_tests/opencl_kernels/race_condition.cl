__kernel void sum_race(__global float* data, __global float* result) {
    size_t id = get_global_id(0);
    *result += data[id];
}

/**
 * Sum with 2 step reduction.
 * Assuming only 1 work group is created and work group size is a power of 2.
 */
__kernel void sum(__global float* data, __local float* tmp_data, uint n, __global float* result) {
    size_t global_id = get_global_id(0);
    size_t local_id = get_local_id(0);
    size_t local_size = get_local_size(0);
    size_t stride = get_global_size(0);
    // assert(local_size == stride); since there is only 1 work group.

    // Step 1: compute partial sum of all elements with
    // index % stride ==  global_id
    float partial_sum = 0.0;
    for (size_t i=global_id; i < n; i+= stride) {
        partial_sum += data[i];
    }
    tmp_data[local_id] = partial_sum;

    barrier(CLK_LOCAL_MEM_FENCE);

    // Step 2: Add up all partial sums by reduction.
    for (size_t offset = local_size/2; offset > 0; offset /= 2) {
        if (local_id < offset) {
            tmp_data[local_id] += tmp_data[local_id + offset];
        }
        barrier(CLK_LOCAL_MEM_FENCE);
    }

    if (local_id == 0) {
        size_t group_id = get_group_id(0);
        result[group_id] = tmp_data[0];
    }
}
