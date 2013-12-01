__kernel void compute_winding_number_v4(
        __global const float4* vertices,
        const uint num_faces,
        __global const float4* points,
        __global float* winding_numbers) {
    size_t i, v_idx;
    float4 tmp1[3], tmp2[3], tmp3[3], tmp4[3], tmp5[3], total_solid_angle, numerator, denominator;
    int id;
    id = get_global_id(0);
#define P tmp1
#define a tmp2
#define b tmp3
#define c tmp4
    P[0] = (float4)(
            points[id*4+0].s0,
            points[id*4+1].s0,
            points[id*4+2].s0,
            points[id*4+3].s0);
    P[1] = (float4)(
            points[id*4+0].s1,
            points[id*4+1].s1,
            points[id*4+2].s1,
            points[id*4+3].s1);
    P[2] = (float4)(
            points[id*4+0].s2,
            points[id*4+1].s2,
            points[id*4+2].s2,
            points[id*4+3].s2);

    total_solid_angle = (float4)(0);

    for (i=0; i<num_faces; i++) {
        v_idx = i*3;
        a[0] = (float4)(vertices[v_idx+0].x) - P[0];
        a[1] = (float4)(vertices[v_idx+0].y) - P[1];
        a[2] = (float4)(vertices[v_idx+0].z) - P[2];
        b[0] = (float4)(vertices[v_idx+1].x) - P[0];
        b[1] = (float4)(vertices[v_idx+1].y) - P[1];
        b[2] = (float4)(vertices[v_idx+1].z) - P[2];
        c[0] = (float4)(vertices[v_idx+2].x) - P[0];
        c[1] = (float4)(vertices[v_idx+2].y) - P[1];
        c[2] = (float4)(vertices[v_idx+2].z) - P[2];

#define a_norm tmp5[0]
#define b_norm tmp5[1]
#define c_norm tmp5[2]
        a_norm = sqrt(a[0]*a[0] + a[1]*a[1] + a[2]*a[2]);
        b_norm = sqrt(b[0]*b[0] + b[1]*b[1] + b[2]*b[2]);
        c_norm = sqrt(c[0]*c[0] + c[1]*c[1] + c[2]*c[2]);
        numerator = 
            a[0]*b[1]*c[2] - a[0]*b[2]*c[1] +
            a[2]*b[0]*c[1] - a[1]*b[0]*c[2] +
            a[1]*b[2]*c[0] - a[2]*b[1]*c[0];
        denominator = a_norm * b_norm * c_norm;
        denominator += (a[0]*b[0] + a[1]*b[1] + a[2]*b[2]) * c_norm;
        denominator += (b[0]*c[0] + b[1]*c[1] + b[2]*c[2]) * a_norm;
        denominator += (c[0]*a[0] + c[1]*a[1] + c[2]*a[2]) * b_norm;

#define angle tmp5[0]
        angle = atan2(numerator, denominator);
        angle /= (float4)(2 * M_PI_F);
        total_solid_angle += angle;
    }
    //vstore4(total_solid_angle, id, winding_numbers);
    winding_numbers[id*4+0] = total_solid_angle.s0;
    winding_numbers[id*4+1] = total_solid_angle.s1;
    winding_numbers[id*4+2] = total_solid_angle.s2;
    winding_numbers[id*4+3] = total_solid_angle.s3;
}

