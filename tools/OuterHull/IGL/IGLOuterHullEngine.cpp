/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "IGLOuterHullEngine.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <sstream>
#include <vector>
#include <Math/MatrixUtils.h>

//#include <igl/cgal/outer_hull.h>
#include <igl/copyleft/cgal/peel_outer_hull_layers.h>
#include <igl/copyleft/cgal/remesh_self_intersections.h>
#include <igl/remove_unreferenced.h>

#include <CGAL/Exact_predicates_exact_constructions_kernel.h>

#include <SelfIntersection/SelfIntersectionResolver.h>

using namespace PyMesh;

namespace IGLOuterHullEngineHelper {
    typedef CGAL::Exact_predicates_exact_constructions_kernel Kernel;
    typedef Kernel::FT ExactScalar;
    typedef Eigen::Matrix<ExactScalar, Eigen::Dynamic, Eigen::Dynamic> MatrixE;
    typedef Eigen::Matrix<ExactScalar, Eigen::Dynamic, Eigen::Dynamic,
            Eigen::RowMajor> MatrixEr;

    void resolve_self_intersections_exactly(
            const MatrixFr& vertices, 
            const MatrixIr& faces,
            MatrixE& out_vertices,
            MatrixI& out_faces,
            VectorI& face_sources) {
        igl::copyleft::cgal::RemeshSelfIntersectionsParam param;
        MatrixIr intersecting_face_pairs;
        VectorI unique_vertex_indices;

        MatrixE tmp_vertices;
        MatrixI tmp_faces;

        igl::copyleft::cgal::remesh_self_intersections(
                vertices,
                faces,
                param,
                tmp_vertices,
                tmp_faces,
                intersecting_face_pairs,
                face_sources,
                unique_vertex_indices);

        std::for_each(tmp_faces.data(),tmp_faces.data()+tmp_faces.size(),
                [&unique_vertex_indices](int & a){
                a=unique_vertex_indices(a);
                });

        igl::remove_unreferenced(tmp_vertices,tmp_faces,
                out_vertices,out_faces,unique_vertex_indices);
    }

    template<typename Derived>
    Derived row_slice(const Eigen::MatrixBase<Derived>& data, const VectorI& indices) {
        const size_t num_rows = indices.rows();
        const size_t num_cols = data.cols();
        Derived result = Derived::Zero(num_rows, num_cols);
        for (size_t i=0; i<num_rows; i++) {
            result.row(i) = data.row(indices[i]);
        }

        return result;
    }

    MatrixF exact_to_float(const MatrixE& data) {
        MatrixF result(data.rows(), data.cols());
        std::transform(data.data(),
                data.data() + data.rows() * data.cols(),
                result.data(),
                [&](const ExactScalar& val) {return CGAL::to_double(val); });
        return result;
    }
}
using namespace IGLOuterHullEngineHelper;

void IGLOuterHullEngine::run() {
    assert(m_vertices.cols() == 3);
    assert(m_faces.cols() == 3);

    extract_outer_hull();
    remove_isolated_vertices();
}

void IGLOuterHullEngine::extract_outer_hull() {
    // resolve self-intersection.
    MatrixE V;
    MatrixI F;
    resolve_self_intersections_exactly(
            m_vertices, m_faces, V, F, m_ori_face_indices);
    m_vertices = exact_to_float(V);

    // Compute outer hull.
    igl::copyleft::cgal::peel_outer_hull_layers(V, F, m_layers, m_face_is_flipped);
    m_faces = F;
}
