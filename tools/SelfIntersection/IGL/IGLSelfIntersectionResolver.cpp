/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "IGLSelfIntersectionResolver.h"

#include <igl/copyleft/cgal/remesh_self_intersections.h>
#include <igl/remove_unreferenced.h>
#include <iostream>

using namespace PyMesh;

void IGLSelfIntersectionResolver::run() {
    typedef CGAL::Exact_predicates_exact_constructions_kernel Kernel;
    typedef Eigen::Matrix<Kernel::FT, Eigen::Dynamic, Eigen::Dynamic,
            Eigen::RowMajor> MatrixEr;
    igl::copyleft::cgal::RemeshSelfIntersectionsParam param;
    MatrixEr out_vertices;
    MatrixIr out_faces;
    MatrixIr intersecting_face_pairs;
    VectorI unique_vertex_indices;

    igl::copyleft::cgal::remesh_self_intersections(
            m_vertices,
            m_faces,
            param,
            out_vertices,
            out_faces,
            intersecting_face_pairs,
            m_face_sources,
            unique_vertex_indices);

    std::for_each(out_faces.data(),out_faces.data()+out_faces.size(),
            [&unique_vertex_indices](int & a){
            a=unique_vertex_indices(a);
            });

    MatrixEr final_vertices;
    igl::remove_unreferenced(out_vertices,out_faces,
            final_vertices,m_faces,unique_vertex_indices);

    m_vertices.resize(final_vertices.rows(), final_vertices.cols());
    std::transform(final_vertices.data(), final_vertices.data() +
            final_vertices.rows() * final_vertices.cols(), m_vertices.data(),
            [](const Kernel::FT& val) { return CGAL::to_double(val); });
}
