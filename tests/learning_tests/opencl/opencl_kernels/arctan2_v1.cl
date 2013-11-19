
__kernel void arctan2_v1(__global float* numerator, __global float* denominator, __global float* results) {
    size_t id = get_global_id(0);
    float y = numerator[id];
    float x = denominator[id];
    results[id] = atan2(y, x);
}
