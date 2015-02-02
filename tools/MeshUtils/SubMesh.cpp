#include "SubMesh.h"

#include <iostream>
#include <algorithm>
#include <memory>
#include <Math/MatrixUtils.h>
#include "IsolatedVertexRemoval.h"

SubMesh::Ptr SubMesh::create(Mesh::Ptr mesh) {
    const size_t dim = mesh->get_dim();
    const size_t num_vertices = mesh->get_num_vertices();
    const size_t num_faces = mesh->get_num_faces();
    const size_t vertex_per_face = mesh->get_vertex_per_face();
    const VectorF& vertices = mesh->get_vertices();
    const VectorI& faces = mesh->get_faces();

    Eigen::Map<const MatrixFr> mat_vertices(vertices.data(), num_vertices, dim);
    Eigen::Map<const MatrixIr> mat_faces(faces.data(), num_faces, vertex_per_face);

    return std::make_shared<SubMesh>(mat_vertices, mat_faces);
}

SubMesh::Ptr SubMesh::create_raw(const MatrixFr& vertices,
        const MatrixIr& faces) {
    return std::make_shared<SubMesh>(vertices, faces);
}

SubMesh::SubMesh(
        const MatrixFr& vertices,
        const MatrixIr& faces)
: m_vertices(vertices), m_faces(faces) {
    const size_t num_vertices = vertices.rows();
    const size_t num_faces = faces.rows();

    m_ori_vertex_indices.resize(num_vertices);
    m_ori_face_indices.resize(num_faces);

    for (size_t i=0; i<num_vertices; i++) {
        m_ori_vertex_indices[i] = i;
    }
    for (size_t i=0; i<num_faces; i++) {
        m_ori_face_indices[i] = i;
    }

    m_vertex_selection = std::vector<bool>(num_vertices, false);
}

void SubMesh::filter_with_vertex_index(
        const std::vector<size_t>& selected_vertex_indices) {
    for (const auto i : selected_vertex_indices) {
        m_vertex_selection[i] = true;
    }
}

void SubMesh::filter_with_face_index(
        const std::vector<size_t>& selected_face_indices) {
    const size_t num_vertex_per_face = m_faces.cols();
    for (const auto i : selected_face_indices) {
        const auto& face = m_faces.row(i);
        for (size_t j=0; j<num_vertex_per_face; j++) {
            m_vertex_selection[face[j]] = true;
        }
    }
}

void SubMesh::filter_vertex_with_custom_function(
        const std::function<bool(const VectorF&)>& func) {
    const size_t num_vertex = m_vertices.rows();
    for (size_t i=0; i<num_vertex; i++) {
        const auto& v = m_vertices.row(i);
        m_vertex_selection[i] = m_vertex_selection[i] || func(v);
    }
}

void SubMesh::finalize() {
    collect_selected_vertices();
    collect_selected_faces();
    remove_isolated_vertices();
}

void SubMesh::collect_selected_vertices() {
    std::vector<int> selection;
    const size_t num_vertices = m_vertices.rows();
    for (size_t i=0; i<num_vertices; i++) {
        if (m_vertex_selection[i]) {
            selection.push_back(i);
        }
    }

    m_ori_vertex_indices = MatrixUtils::std2eigen(selection);
}

void SubMesh::collect_selected_faces() {
    std::vector<int> selection;
    std::vector<VectorI> selected_faces;
    const size_t num_faces = m_faces.rows();
    const size_t num_vertex_per_face = m_faces.cols();
    for (size_t i=0; i<num_faces; i++) {
        const auto& face = m_faces.row(i);
        bool selected = true;
        for (size_t j=0; selected && j<num_vertex_per_face; j++) {
            selected = selected && m_vertex_selection[face[j]];
        }
        if (selected) {
            selection.push_back(i);
            selected_faces.push_back(face);
        }
    }

    m_ori_face_indices = MatrixUtils::std2eigen(selection);
    m_faces = MatrixUtils::rowstack(selected_faces);
}

void SubMesh::remove_isolated_vertices() {
    IsolatedVertexRemoval remover(m_vertices, m_faces);
    remover.run();
    m_vertices = remover.get_vertices();
    m_faces = remover.get_faces();
}

