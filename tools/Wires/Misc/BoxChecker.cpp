/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "BoxChecker.h"

#include <sstream>
#include <Core/Exception.h>

using namespace PyMesh;

BoxChecker::BoxChecker(const VectorF& bbox_min, const VectorF& bbox_max) :
    m_bbox_min(bbox_min), m_bbox_max(bbox_max) {
        if (!(m_bbox_max.array() >= m_bbox_min.array()).all()) {
            std::stringstream err_msg;
            err_msg << "Invalid bbox!" << std::endl;
            err_msg << "\tbbox_min: " << m_bbox_min.transpose() << std::endl;
            err_msg << "\tbbox_max: " << m_bbox_max.transpose() << std::endl;
            throw RuntimeError(err_msg.str());
        }

        m_bbox_size = m_bbox_max - m_bbox_min;
        m_dim = m_bbox_size.size();
        m_tol = 1e-12;
    }

bool BoxChecker::is_inside(const VectorF& p) const {
    bool r = true;
    for (size_t i=0; i<m_dim; i++) {
        r = r && (p[i] >= m_bbox_min[i] + m_tol);
        r = r && (p[i] <= m_bbox_max[i] - m_tol);
    }
    return r;
}

bool BoxChecker::is_outside(const VectorF& p) const {
    bool r = false;
    for (size_t i=0; i<m_dim; i++) {
        r = r || (p[i] <= m_bbox_min[0] - m_tol);
        r = r || (p[i] >= m_bbox_max[0] + m_tol);
    }
    return r;
}

bool BoxChecker::is_on_boundary(const VectorF& p) const {
    return (!is_inside(p)) && (!is_outside(p));
}

bool BoxChecker::is_on_boundary_edges(const VectorF& p) const {
    if (!is_on_boundary(p)) return false;
    return get_dim_on_border(p) > 1;
}

bool BoxChecker::is_on_boundary_corners(const VectorF& p) const {
    if (!is_on_boundary(p)) return false;
    return get_dim_on_border(p) == m_dim;
}

size_t BoxChecker::get_dim_on_border(const VectorF& p) const {
    VectorF dist_to_min = p - m_bbox_min;
    size_t num_dim_on_border = 0;
    for (size_t i=0; i<m_dim; i++) {
        if (fabs(dist_to_min[i]                 ) < m_tol ||
            fabs(dist_to_min[i] - m_bbox_size[i]) < m_tol) {
            num_dim_on_border++;
        }
    }

    return num_dim_on_border;
}
