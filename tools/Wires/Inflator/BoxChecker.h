#pragma once

#include <Core/EigenTypedef.h>

class BoxChecker {
    public:
        BoxChecker(const Vector3F& bbox_min, const Vector3F& bbox_max);

    public:
        void set_tolerance(Float tol) { m_tol = tol; }

        bool is_inside(const Vector3F& p);
        bool is_outside(const Vector3F& p);
        bool is_on_boundary(const Vector3F& p);
        bool is_on_boundary_edges(const Vector3F& p);
        bool is_on_boundary_corners(const Vector3F& p);

    private:
        size_t get_dim_on_border(const Vector3F& p);

    private:
        Vector3F m_bbox_min;
        Vector3F m_bbox_max;
        Vector3F m_bbox_size;
        Float m_tol;
};
