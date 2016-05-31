/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "SubMesh.h"

#include <iostream>
#include <algorithm>
#include <memory>
#include <sstream>
#include <Math/MatrixUtils.h>
#include "IsolatedVertexRemoval.h"

using namespace PyMesh;

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
    collect_selected_faces();
    clean_up_selected();
    clean_up_unselected();
}

void SubMesh::collect_selected_faces() {
    std::vector<int> selected, unselected;
    std::vector<VectorI> selected_faces, unselected_faces;
    const size_t num_faces = m_faces.rows();
    const size_t num_vertex_per_face = m_faces.cols();
    for (size_t i=0; i<num_faces; i++) {
        const auto& face = m_faces.row(i);
        bool is_selected = true;
        for (size_t j=0; is_selected && j<num_vertex_per_face; j++) {
            is_selected = is_selected && m_vertex_selection[face[j]];
        }
        if (is_selected) {
            selected.push_back(i);
            selected_faces.push_back(face);
        } else {
            unselected.push_back(i);
            unselected_faces.push_back(face);
        }
    }

    assert(selected.size() + unselected.size() == num_faces);
    assert(selected_faces.size() + unselected_faces.size() == num_faces);
    if (selected.size() > 0) {
        m_selected_face_indices = MatrixUtils::std2eigen(selected);
        m_selected_faces = MatrixUtils::rowstack(selected_faces);
    } else {
        m_selected_face_indices.resize(0);
        m_selected_faces.resize(0, num_vertex_per_face);
    }

    if (unselected.size() > 0) {
        m_unselected_face_indices = MatrixUtils::std2eigen(unselected);
        m_unselected_faces = MatrixUtils::rowstack(unselected_faces);
    } else {
        m_unselected_face_indices.resize(0);
        m_unselected_faces.resize(0, num_vertex_per_face);
    }
}

void SubMesh::clean_up_selected() {
    IsolatedVertexRemoval remover(m_vertices, m_selected_faces);
    remover.run();
    m_selected_vertices = remover.get_vertices();
    m_selected_faces = remover.get_faces();
    m_selected_vertex_indices = remover.get_ori_vertex_indices();
}

void SubMesh::clean_up_unselected() {
    IsolatedVertexRemoval remover(m_vertices, m_unselected_faces);
    remover.run();
    m_unselected_vertices = remover.get_vertices();
    m_unselected_faces = remover.get_faces();
    m_unselected_vertex_indices = remover.get_ori_vertex_indices();
}

void SubMesh::check_validity() const {
    if (m_selected_faces.rows() + m_unselected_faces.rows() != m_faces.rows()) {
        std::stringstream err_msg;
        err_msg << "Out of " << m_faces.rows() << " faces, "
            << m_selected_faces.rows() << " faces are selected, "
            << m_unselected_faces.rows() << " face are unselected, "
            << m_faces.rows() - m_selected_faces.rows() -
            m_unselected_faces.rows() << " are missing." << std::endl;
        err_msg << "Did you forget to call finalize()?";
        throw RuntimeError(err_msg.str());
    }
}

