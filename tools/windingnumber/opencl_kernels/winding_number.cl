
__kernel void compute_winding_number(
        const uint num_vertices,
        __global const float* vertices,
        const uint num_faces,
        __global const int* faces,
        const uint num_pts,
        __global const float* points,
        __global float* winding_numbers) {
    const int id = get_global_id(0);
    float p[3] = {
        points[id*3 + 0],
        points[id*3 + 1],
        points[id*3 + 2]};
    float total_solid_angle = 0;

    for (size_t i=0; i<num_faces; i++) {
        size_t v1_idx = faces[i*3 + 0];
        size_t v2_idx = faces[i*3 + 1];
        size_t v3_idx = faces[i*3 + 2];
        float v1[3] = {
            vertices[v1_idx*3 + 0],
            vertices[v1_idx*3 + 1],
            vertices[v1_idx*3 + 2] };
        float v2[3] = {
            vertices[v2_idx*3 + 0],
            vertices[v2_idx*3 + 1],
            vertices[v2_idx*3 + 2] };
        float v3[3] = {
            vertices[v3_idx*3 + 0],
            vertices[v3_idx*3 + 1],
            vertices[v3_idx*3 + 2] };
        float a[3] = {
            v1[0] - p[0],
            v1[1] - p[1],
            v1[2] - p[2]};
        float b[3] = {
            v2[0] - p[0],
            v2[1] - p[1],
            v2[2] - p[2]};
        float c[3] = {
            v3[0] - p[0],
            v3[1] - p[1],
            v3[2] - p[2]};
        float a_norm = sqrt(a[0]*a[0] + a[1]*a[1] + a[2]*a[2]);
        float b_norm = sqrt(b[0]*b[0] + b[1]*b[1] + b[2]*b[2]);
        float c_norm = sqrt(c[0]*c[0] + c[1]*c[1] + c[2]*c[2]);
        float a_dot_b = a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
        float b_dot_c = b[0]*c[0] + b[1]*c[1] + b[2]*c[2];
        float c_dot_a = c[0]*a[0] + c[1]*a[1] + c[2]*a[2];
        float det =
            a[0]*b[1]*c[2] - a[0]*b[2]*c[1] +
            a[2]*b[0]*c[1] - a[1]*b[0]*c[2] +
            a[1]*b[2]*c[0] - a[2]*b[1]*c[0];
        float denominator =
            a_norm * b_norm * c_norm +
            a_dot_b * c_norm +
            b_dot_c * a_norm +
            c_dot_a * b_norm;
        float angle = atan2(det, denominator) * 2;
        total_solid_angle += angle;
    }
    total_solid_angle /= (4 * M_PI_F);
    winding_numbers[id] = total_solid_angle;
    //winding_numbers[id] = 0;
}

/*
__kernel void test(__global uint* result) {
    const size_t id = get_global_id(0);
    result[id] = id;
}
*/

__kernel void compute_solid_angles(
        const uint num_vertices,
        __global const float3* vertices,
        const uint num_faces,
        __global const int3* faces,
        const uint num_pts,
        __global const float3* points,
        __global float* winding_numbers) {
    const int id = get_global_id(0);
    const size_t fid = id / num_pts;
    const size_t pid = id % num_pts;
    winding_numbers[pid] = points[pid].s0;
    /*
    if (fid < num_faces) {
        float3 p  = points[pid];
        float3 v1 = vertices[faces[fid].s0];
        float3 v2 = vertices[faces[fid].s1];
        float3 v3 = vertices[faces[fid].s2];
        float3 a = v1 - p;
        float3 b = v2 - p;
        float3 c = v3 - p;
        float a_norm = length(a);
        float b_norm = length(b);
        float c_norm = length(c);
        float det = dot(a, cross(b, c));
        float denominator = a_norm * b_norm * c_norm +
            dot(a, b) * c_norm +
            dot(b, c) * a_norm +
            dot(c, a) * b_norm;
        float angle = atan2(det, denominator) * 2;
        angle /= 4 * M_PI_F;
        winding_numbers[pid] += angle;
    }
    */
}


