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
