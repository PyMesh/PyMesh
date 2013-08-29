#include "WindingNumber.h"
#include <cmath>
#include <Mesh.h>

Float compute_winding_num_at_point(const VectorF& v, const VectorI& f, const Vector3F& p) {
    Float total_solid_angle=0;
    for (size_t i=0; i<f.size(); i+=3) {
        Vector3F a = v.segment(f[i  ]*3,3) - p;
        Vector3F b = v.segment(f[i+1]*3,3) - p;
        Vector3F c = v.segment(f[i+2]*3,3) - p;
        Float na = a.norm();
        Float nb = b.norm();
        Float nc = c.norm();

        Float det =
            a[0]*b[1]*c[2] - a[0]*b[2]*c[1] +
            a[1]*b[2]*c[0] - a[1]*b[0]*c[2] +
            a[2]*b[0]*c[1] - a[2]*b[1]*c[0];
        Float angle = 2 * atan2(det,
                na*nb*nc + a.dot(b)*nc + a.dot(c)*nb + b.dot(c)*na);
        total_solid_angle += angle;
    }
    return total_solid_angle / (4 * M_PI);
}

VectorF WindingNumber::compute(const VectorF& pts) {
    VectorF result(pts.size() / 3);
    for (size_t i=0; i<pts.size(); i+=3) {
        const Vector3F& p = pts.segment(i,3);
        result[i/3] = compute_winding_num_at_point(
                m_mesh.get_vertices(), m_mesh.get_faces(), p);
    }
    return result;
}
