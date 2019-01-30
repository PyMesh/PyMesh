/* This file is part of PyMesh. Copyright (c) 2017 by Qingnan Zhou */
#pragma once
#ifdef WITH_IGL

#include <atomic>
#include <Core/EigenTypedef.h>
#include <igl/AABB.h>
#include <igl/read_triangle_mesh.h>
#include <igl/signed_distance.h>
#include <igl/copyleft/cgal/assign_scalar.h>

namespace PyMesh {

template<typename DerivedV, typename DerivedF>
class Oracle {
    public:
        using Scalar = typename DerivedV::Scalar;
        using Index  = typename DerivedF::Scalar;
        using EigenPoint = Eigen::Matrix<Scalar, 1, 3>;

    public:
        Oracle(Eigen::PlainObjectBase<DerivedV>& V,
                Eigen::PlainObjectBase<DerivedF>& F): m_vertices(V), m_faces(F)/*, m_counter(0)*/ {
            assert(m_vertices.cols() == 3);
            m_aabb.init(m_vertices, m_faces);
            m_winding_number_hierarchy.set_mesh(m_vertices, m_faces);
            m_winding_number_hierarchy.grow();
        }

    public:
        template<typename Point>
        Scalar operator()(const Point& p) const {
            //m_counter++;
            EigenPoint point;
            igl::copyleft::cgal::assign_scalar(p[0], point[0]);
            igl::copyleft::cgal::assign_scalar(p[1], point[1]);
            igl::copyleft::cgal::assign_scalar(p[2], point[2]);
            return igl::signed_distance_winding_number(m_aabb, m_vertices,
                    m_faces, m_winding_number_hierarchy, point);
        }

        //size_t get_counter() const { return m_counter; }

    private:
        Eigen::PlainObjectBase<DerivedV>& m_vertices;
        Eigen::PlainObjectBase<DerivedF>& m_faces;
        igl::AABB<DerivedV, 3> m_aabb;
        igl::WindingNumberAABB<EigenPoint, DerivedV, DerivedF> m_winding_number_hierarchy;
        //mutable std::atomic<size_t> m_counter;
        //mutable size_t m_counter;
};

}

#endif
