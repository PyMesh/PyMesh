__kernel void square(__global float* data, __global float* results) {
    size_t id = get_global_id(0);
    results[id] = data[id] * data[id];
}

__kernel void square_vec2(__global float2* data, __global float2* results) {
    size_t id = get_global_id(0);
    results[id] = data[id] * data[id];
}

__kernel void square_vec4(__global float4* data, __global float4* results) {
    size_t id = get_global_id(0);
    results[id] = data[id] * data[id];
}

__kernel void square_vec8(__global float8* data, __global float8* results) {
    size_t id = get_global_id(0);
    results[id] = data[id] * data[id];
}

__kernel void square_vec16(__global float16* data, __global float16* results) {
    size_t id = get_global_id(0);
    results[id] = data[id] * data[id];
}

/**
 * This kernel would fail because float3 is 16 bytes aligned on some devices.
 * Assumes the input C array is like the following:
 *   d0  d1  d2  d3  d4  d5  d6  d7  ...
 *   \        /      \        /
 *    data[0]         data[1]
 * Note that d3, d7, ... are skipped.
 * Eventually, this kernel will access invalid memory and caused a seg fault.
 * To test it, we only perform the task for the two vectors.
 */
__kernel void square_vec3(__global float3* data, __global float3* results) {
    size_t id = get_global_id(0);
    if (id <= 1)
        results[id] = data[id] * data[id];
}

/**
 * Explicitly construct float3 from data.
 */
__kernel void square_vec3_explicit(__global float* data, __global float* results) {
    size_t id = get_global_id(0);
    float3 x = (float3)(
            data[id*3 + 0],
            data[id*3 + 1],
            data[id*3 + 2]);
    float3 y = x*x;
    results[id*3 + 0] = y.s0;
    results[id*3 + 1] = y.s1;
    results[id*3 + 2] = y.s2;
}
