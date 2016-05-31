/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <Core/EigenTypedef.h>

namespace PyMesh {

class BoxChecker {
    public:
        BoxChecker(const VectorF& bbox_min, const VectorF& bbox_max);

    public:
        void set_tolerance(Float tol) { m_tol = tol; }

        bool is_inside(const VectorF& p) const;
        bool is_outside(const VectorF& p) const;
        bool is_on_boundary(const VectorF& p) const;
        bool is_on_boundary_edges(const VectorF& p) const;
        bool is_on_boundary_corners(const VectorF& p) const;

    private:
        size_t get_dim_on_border(const VectorF& p) const;

    private:
        size_t  m_dim;
        VectorF m_bbox_min;
        VectorF m_bbox_max;
        VectorF m_bbox_size;
        Float m_tol;
};

}
