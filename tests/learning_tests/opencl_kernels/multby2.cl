__kernel void multby2(__global float* data, __global float* results) {
    size_t id = get_global_id(0);
    results[id] = data[id] * 2;
}

