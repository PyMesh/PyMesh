
__kernel void arctan2_v2(__global float8* numerator, __global float8* denominator, __global float8* results) {
    size_t id = get_global_id(0);
    float8 y = numerator[id];
    float8 x = denominator[id];
    results[id] = atan2(y, x);
}
