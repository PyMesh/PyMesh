#pragma once

#include <Core/EigenTypedef.h>

class BoxChecker {
    public:
        BoxChecker(const VectorF& bbox_min, const VectorF& bbox_max);

    public:
        void set_tolerance(Float tol) { m_tol = tol; }

        bool is_inside(const VectorF& p);
        bool is_outside(const VectorF& p);
        bool is_on_boundary(const VectorF& p);
        bool is_on_boundary_edges(const VectorF& p);
        bool is_on_boundary_corners(const VectorF& p);

    private:
        size_t get_dim_on_border(const VectorF& p);

    private:
        size_t  m_dim;
        VectorF m_bbox_min;
        VectorF m_bbox_max;
        VectorF m_bbox_size;
        Float m_tol;
};
