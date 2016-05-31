/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "IsolatedVertexRemoval.h"
#include <cassert>
#include <vector>

#include <Math/MatrixUtils.h>

using namespace PyMesh;

IsolatedVertexRemoval::IsolatedVertexRemoval(
        const MatrixFr& vertices, const MatrixIr& faces) :
    m_vertices(vertices), m_faces(faces) { }

size_t IsolatedVertexRemoval::run() {
    const size_t num_vertices = m_vertices.rows();
    const size_t num_faces = m_faces.rows();
    const size_t dim = m_vertices.cols();
    const size_t vertex_per_face = m_faces.cols();

    std::vector<bool> is_isolated(num_vertices, true);
    for (size_t i=0; i<num_faces; i++) {
        for (size_t j=0; j<vertex_per_face; j++) {
            is_isolated[m_faces.coeff(i,j)] = false;
        }
    }

    std::vector<int> ori_vertex_indices;
    VectorI index_map = VectorI::Ones(num_vertices) * -1;
    for (size_t i=0; i<num_vertices; i++) {
        if (is_isolated[i]) continue;
        index_map[i] = ori_vertex_indices.size();
        ori_vertex_indices.push_back(i);
    }
    m_ori_vertex_indices = MatrixUtils::std2eigen(ori_vertex_indices);

    const size_t num_vertices_after = ori_vertex_indices.size();
    MatrixFr vertices(num_vertices_after, dim);
    for (size_t i=0; i<num_vertices_after; i++) {
        vertices.row(i) = m_vertices.row(m_ori_vertex_indices[i]);
    }
    m_vertices = vertices;

    for (size_t i=0; i<num_faces; i++) {
        for (size_t j=0; j<vertex_per_face; j++) {
            assert(!is_isolated[m_faces.coeff(i,j)]);
            m_faces.coeffRef(i,j) = index_map[m_faces.coeff(i,j)];
        }
    }

    return num_vertices - num_vertices_after;
}
