__kernel void multby2(__global float* data, __global float* results) {
    size_t id = get_global_id(0);
    results[id] = data[id] * 2;
}

__kernel void multby2_v2(__global float2* data, __global float2* results) {
    size_t id = get_global_id(0);
    float2 x = data[id];
    // x *= 2; // This would cause a build error.
    x.s0 *= 2;
    x.s1 *= 2;
    results[id] = x;
}

__kernel void multby2_v3(__global float* data, __global float* results) {
    /*
    const size_t SIZE = 512;
    size_t id = get_global_id(0);
    float8 tmp[SIZE];
    tmp[0] = vload8(id, data);
    for (size_t i=1; i<SIZE; i++) {
        tmp[i] = tmp[i-1] + tmp[0];
    }
    float8 y = tmp[0] + tmp[0];
    vstore8(y, id, results);
    */
}
