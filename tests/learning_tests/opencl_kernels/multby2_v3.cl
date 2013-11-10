
#define N 512*1024
__kernel void multby2_v3(__global float* data, __global float* results) {
    size_t id = get_global_id(0);
    float8 tmp[N];
    tmp[0] = vload8(id, data);
    for (size_t i=1; i<N; i++) {
        tmp[i] = tmp[i-1] + tmp[0];
    }
    float8 y = tmp[0] + tmp[0];
    vstore8(y, id, results);
}
