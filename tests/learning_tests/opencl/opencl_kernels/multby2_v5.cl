
__kernel void multby2_v5(__global float* data, __global float* results) {
    size_t id = get_global_id(0);
    __local float x;
    x = data[id];
    results[id] = x*2;
}

