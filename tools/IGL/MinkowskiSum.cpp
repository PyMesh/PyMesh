/* This file is part of PyMesh. Copyright (c) 2016 by Qingnan Zhou */
#include "MinkowskiSum.h"
#include <Math/MatrixUtils.h>
#include <igl/copyleft/cgal/minkowski_sum.h>
#include <igl/copyleft/cgal/mesh_boolean.h>
#include <igl/MeshBooleanType.h>

#include <vector>

using namespace PyMesh;

MinkowskiSum::Ptr MinkowskiSum::create(const Mesh::Ptr& mesh) {
    const MatrixFr vertices = MatrixUtils::reshape<MatrixFr>(
            mesh->get_vertices(), mesh->get_num_vertices(), mesh->get_dim());
    const MatrixIr faces = MatrixUtils::reshape<MatrixIr>(
            mesh->get_faces(), mesh->get_num_faces(),
            mesh->get_vertex_per_face());
    return MinkowskiSum::Ptr(new MinkowskiSum(vertices, faces));
}

MinkowskiSum::Ptr MinkowskiSum::create_raw(
        const MatrixFr& vertices, const MatrixIr& faces) {
    return MinkowskiSum::Ptr(new MinkowskiSum(vertices, faces));
}

void MinkowskiSum::run(const MatrixFr& path) {
    const size_t num_pts = path.rows();
    if (num_pts <= 1) {
        m_out_vertices = m_vertices;
        m_out_faces = m_faces;
        return;
    }

    std::vector<MatrixFr> vertices;
    std::vector<MatrixIr> faces;

    for (size_t i=1; i<num_pts; i++) {
        const Eigen::Matrix<Float, 1, 3> s = path.row(i-1);
        const Eigen::Matrix<Float, 1, 3> d = path.row(i);
        MatrixFr V;
        MatrixIr F;
        VectorI J;

        igl::copyleft::cgal::minkowski_sum(m_vertices, m_faces,
                s, d, V, F, J);
        vertices.emplace_back(V);
        faces.emplace_back(F);
    }

    size_t v_count = 0;
    for (size_t i=0; i<num_pts-1; i++) {
        faces[i].array() += v_count;
        v_count += vertices[i].rows();
    }

    MatrixFr combined_vertices = MatrixUtils::vstack(vertices);
    MatrixIr combined_faces = MatrixUtils::vstack(faces);

    // Self union to remove self-intersections.
    MatrixFr empty_vertices;
    MatrixIr empty_faces;
    VectorI J;

    igl::copyleft::cgal::mesh_boolean(
            combined_vertices, combined_faces,
            empty_vertices, empty_faces,
            igl::MESH_BOOLEAN_TYPE_UNION,
            m_out_vertices, m_out_faces, J);
}
