__kernel void compute_solid_angles(
        __global const float4* vertices,
        const uint num_faces,
        const uint num_pts,
        __global const float4* points,
        __global float* winding_numbers) {
    const int fid = get_global_id(0);
    const int pid = get_global_id(1);
    if (fid < num_faces) {
        float4 p = points[pid];
        float4 a = vertices[fid*3 + 0] - p;
        float4 b = vertices[fid*3 + 1] - p;
        float4 c = vertices[fid*3 + 2] - p;
        float a_norm = length(a);
        float b_norm = length(b);
        float c_norm = length(c);
        float det = dot(a, cross(b, c));
        float denominator = a_norm * b_norm * c_norm +
            dot(a, b) * c_norm +
            dot(b, c) * a_norm +
            dot(c, a) * b_norm;
        float angle = atan2(det, denominator) * 2;
        winding_numbers[pid] += angle;
    }
}
