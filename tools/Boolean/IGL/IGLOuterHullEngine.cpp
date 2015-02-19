#include "IGLOuterHullEngine.h"

#include <cassert>
#include <iostream>
#include <vector>
#include <Math/MatrixUtils.h>

#include <igl/cgal/remesh_self_intersections.h>
#include <igl/outer_hull.h>

void IGLOuterHullEngine::run() {
    assert(m_vertices.cols() == 3);
    assert(m_faces.cols() == 3);

    extract_face_normals();
    resolve_self_intersections();
    extract_outer_hull();
}

void IGLOuterHullEngine::extract_face_normals() {
    const size_t num_faces = m_faces.rows();
    m_normals = Matrix3Fr::Zero(num_faces, 3);
    for (size_t i=0; i<num_faces; i++) {
        const auto& f = m_faces.row(i);
        Vector3F v0 = m_vertices.row(f[0]);
        Vector3F v1 = m_vertices.row(f[1]);
        Vector3F v2 = m_vertices.row(f[2]);
        m_normals.row(i) = ((v1-v0).cross(v2-v0)).normalized();
    }
}

void IGLOuterHullEngine::resolve_self_intersections() {
    igl::RemeshSelfIntersectionsParam param;
    MatrixFr out_vertices;
    MatrixIr out_faces;
    MatrixIr intersecting_face_pairs;
    VectorI ori_face_indices;
    VectorI unique_vertex_indices;

    igl::remesh_self_intersections(
            m_vertices,
            m_faces,
            param,
            out_vertices,
            out_faces,
            intersecting_face_pairs,
            ori_face_indices,
            unique_vertex_indices);

    m_vertices = out_vertices;
    m_faces = out_faces;

    const size_t num_out_faces = out_faces.rows();
    Matrix3Fr out_normals = Matrix3Fr::Zero(num_out_faces, 3);
    for (size_t i=0; i<num_out_faces; i++) {
        out_normals.row(i) = m_normals.row(ori_face_indices[i]);
    }
    m_normals = out_normals;
}

void IGLOuterHullEngine::extract_outer_hull() {
    assert(m_faces.rows() == m_normals.rows());
    MatrixIr out_faces;
    VectorI ori_face_indices;
    VectorI ori_face_is_flipped;
    igl::outer_hull(
            m_vertices,
            m_faces,
            m_normals,
            out_faces,
            ori_face_indices,
            ori_face_is_flipped);

    const size_t num_faces = m_faces.rows();
    const size_t num_out_faces = out_faces.rows();
    assert(ori_face_indices.size() == num_out_faces);
    std::vector<bool> in_outer(num_faces, false);
    for (size_t i=0; i<num_out_faces; i++) {
        in_outer[ori_face_indices[i]] = true;
    }

    std::vector<VectorI> interior_faces;
    for (size_t i=0; i<num_faces; i++) {
        if (in_outer[i]) continue;
        interior_faces.push_back(m_faces.row(i));
    }

    m_faces = out_faces;
    if (interior_faces.size() > 0) {
        m_interior_faces = MatrixUtils::rowstack(interior_faces);
    }
    m_interior_vertices = m_vertices;
}
