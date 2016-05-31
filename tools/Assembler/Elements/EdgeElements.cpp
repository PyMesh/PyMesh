/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "EdgeElements.h"

#include <Core/Exception.h>
#include <MeshUtils/Boundary.h>

using namespace PyMesh;

EdgeElements::EdgeElements(Mesh::Ptr mesh) : m_mesh(mesh) {
    check_mesh();
    extract_boundary_edges();
    compute_edge_lengths();
}

size_t EdgeElements::getDim() const {
    return m_mesh->get_dim();
}

size_t EdgeElements::getNbrNodes() const {
    return m_mesh->get_num_vertices();
}

VectorF EdgeElements::getNodes() const {
    return m_mesh->get_vertices();
}

VectorF EdgeElements::getNode(size_t vi) const {
    return m_mesh->get_vertex(vi);
}

size_t EdgeElements::getNbrElements() const {
    return m_edges.rows();
}

size_t EdgeElements::getNodePerElement() const {
    // Edge always contains 2 nodes.
    return 2;
}

VectorI EdgeElements::getElements() const {
    const size_t num_entries = m_edges.rows() * m_edges.cols();
    VectorI edges(num_entries);
    std::copy(m_edges.data(), m_edges.data() + num_entries, edges.data());
    return edges;
}

VectorI EdgeElements::getElement(size_t ei) const {
    return m_edges.row(ei);
}

Float EdgeElements::getElementVolume(size_t ei) const {
    return m_edge_lengths[ei];
}

VectorF EdgeElements::getElementCenter(size_t ei) const {
    VectorF v1 = m_mesh->get_vertex(m_edges(ei, 0));
    VectorF v2 = m_mesh->get_vertex(m_edges(ei, 1));
    return 0.5 * (v1 + v2);
}

void EdgeElements::check_mesh() {
    if (m_mesh->get_num_faces() == 0) {
        throw RuntimeError("Mesh contains no face!");
    }
}

void EdgeElements::extract_boundary_edges() {
    Boundary::Ptr boundary = Boundary::extract_surface_boundary(*m_mesh.get());
    m_edges = boundary->get_boundaries();
}

void EdgeElements::compute_edge_lengths() {
    size_t num_elements = getNbrElements();
    m_edge_lengths.resize(num_elements);
    for (size_t i=0; i<num_elements; i++) {
        VectorI edge = getElement(i);
        VectorF v1 = getNode(edge[0]);
        VectorF v2 = getNode(edge[1]);
        m_edge_lengths[i] = (v1 - v2).norm();
    }
}

