#include "BoxChecker.h"

#include <sstream>
#include <Core/Exception.h>

BoxChecker::BoxChecker(const Vector3F& bbox_min, const Vector3F& bbox_max) :
    m_bbox_min(bbox_min), m_bbox_max(bbox_max) {
        if (!(m_bbox_max.array() >= m_bbox_min.array()).all()) {
            std::stringstream err_msg;
            err_msg << "Invalid bbox!" << std::endl;
            err_msg << "\tbbox_min: " << m_bbox_min.transpose() << std::endl;
            err_msg << "\tbbox_max: " << m_bbox_max.transpose() << std::endl;
            throw RuntimeError(err_msg.str());
        }

        m_bbox_size = m_bbox_max - m_bbox_min;
        m_tol = 1e-12;
    }

bool BoxChecker::is_inside(const Vector3F& p) {
    return ((p[0] >= m_bbox_min[0] + m_tol && p[0] <= m_bbox_max[0] - m_tol) &&
            (p[1] >= m_bbox_min[1] + m_tol && p[1] <= m_bbox_max[1] - m_tol) &&
            (p[2] >= m_bbox_min[2] + m_tol && p[2] <= m_bbox_max[2] - m_tol));
}

bool BoxChecker::is_outside(const Vector3F& p) {
    return ((p[0] <= m_bbox_min[0] - m_tol || p[0] >= m_bbox_max[0] + m_tol) ||
            (p[1] <= m_bbox_min[1] - m_tol || p[1] >= m_bbox_max[1] + m_tol) ||
            (p[2] <= m_bbox_min[2] - m_tol || p[2] >= m_bbox_max[2] + m_tol));
}

bool BoxChecker::is_on_boundary(const Vector3F& p) {
    return ((
                (p[0] < m_bbox_min[0] + m_tol || p[0] > m_bbox_max[0] - m_tol) ||
                (p[1] < m_bbox_min[1] + m_tol || p[1] > m_bbox_max[1] - m_tol) ||
                (p[2] < m_bbox_min[2] + m_tol || p[2] > m_bbox_max[2] - m_tol)
            ) && (
                (p[0] > m_bbox_min[0] - m_tol && p[0] < m_bbox_max[0] + m_tol) &&
                (p[1] > m_bbox_min[1] - m_tol && p[1] < m_bbox_max[1] + m_tol) &&
                (p[2] > m_bbox_min[2] - m_tol && p[2] < m_bbox_max[2] + m_tol)
           ));
}

bool BoxChecker::is_on_boundary_edges(const Vector3F& p) {
    if (!is_on_boundary(p)) return false;
    return get_dim_on_border(p) > 1;
}

bool BoxChecker::is_on_boundary_corners(const Vector3F& p) {
    if (!is_on_boundary(p)) return false;
    return get_dim_on_border(p) == 3;
}

size_t BoxChecker::get_dim_on_border(const Vector3F& p) {
    Vector3F dist_to_min = p - m_bbox_min;
    size_t num_dim_on_border = 0;
    if (fabs(dist_to_min[0]                 ) < m_tol ||
        fabs(dist_to_min[0] - m_bbox_size[0]) < m_tol)
        num_dim_on_border++;
    if (fabs(dist_to_min[1]                 ) < m_tol ||
        fabs(dist_to_min[1] - m_bbox_size[1]) < m_tol)
        num_dim_on_border++;
    if (fabs(dist_to_min[2]                 ) < m_tol ||
        fabs(dist_to_min[2] - m_bbox_size[2]) < m_tol)
        num_dim_on_border++;
    return num_dim_on_border;
}
