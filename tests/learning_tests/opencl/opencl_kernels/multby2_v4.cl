
__kernel void multby2_v4(__global float* data, __global float* results) {
    size_t id = get_global_id(0);
    float x = data[id + 1] - 1;
    results[id] = x*2;
}

