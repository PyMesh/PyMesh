__kernel void compute_winding_number_v4(
        __global const float4* vertices,
        const uint num_faces,
        __global const float4* points,
        __global float* winding_numbers) {
    const int id = get_global_id(0);
    float4 p[3] = {
        (float4)(points[id*4+0].s0,
                 points[id*4+1].s0,
                 points[id*4+2].s0,
                 points[id*4+3].s0),
        (float4)(points[id*4+0].s1,
                 points[id*4+1].s1,
                 points[id*4+2].s1,
                 points[id*4+3].s1),
        (float4)(points[id*4+0].s2,
                 points[id*4+1].s2,
                 points[id*4+2].s2,
                 points[id*4+3].s2)};

    float4 total_solid_angle = 0;

    for (size_t i=0; i<num_faces; i++) {
        float4 p1 = vertices[i*3+0];
        float4 p2 = vertices[i*3+1];
        float4 p3 = vertices[i*3+2];

        float4 a[3] = {(float4)(p1.x), (float4)(p1.y), (float4)(p1.z)};
        float4 b[3] = {(float4)(p2.x), (float4)(p2.y), (float4)(p2.z)};
        float4 c[3] = {(float4)(p3.x), (float4)(p3.y), (float4)(p3.z)};
        a[0] -= p[0]; a[1] -= p[1]; a[2] -= p[2];
        b[0] -= p[0]; b[1] -= p[1]; b[2] -= p[2];
        c[0] -= p[0]; c[1] -= p[1]; c[2] -= p[2];

        float4 a_norm = sqrt(a[0]*a[0] + a[1]*a[1] + a[2]*a[2]);
        float4 b_norm = sqrt(b[0]*b[0] + b[1]*b[1] + b[2]*b[2]);
        float4 c_norm = sqrt(c[0]*c[0] + c[1]*c[1] + c[2]*c[2]);
        float4 a_dot_b = a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
        float4 b_dot_c = b[0]*c[0] + b[1]*c[1] + b[2]*c[2];
        float4 c_dot_a = c[0]*a[0] + c[1]*a[1] + c[2]*a[2];
        float4 det =
            a[0]*b[1]*c[2] - a[0]*b[2]*c[1] +
            a[2]*b[0]*c[1] - a[1]*b[0]*c[2] +
            a[1]*b[2]*c[0] - a[2]*b[1]*c[0];
        float4 denominator =
            a_norm * b_norm * c_norm +
            a_dot_b * c_norm +
            b_dot_c * a_norm +
            c_dot_a * b_norm;
        float4 angle = atan2(det, denominator);
        angle /= (float4)(2 * M_PI_F);
        total_solid_angle += angle;
    }
    vstore4(total_solid_angle, id, winding_numbers);
}

