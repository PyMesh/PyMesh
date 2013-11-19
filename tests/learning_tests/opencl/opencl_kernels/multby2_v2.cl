
__kernel void multby2_v2(__global float2* data, __global float2* results) {
    size_t id = get_global_id(0);
    float2 x = data[id];
    // x *= 2; // This would cause a build error.
    x.s0 *= 2;
    x.s1 *= 2;
    results[id] = x;
}

