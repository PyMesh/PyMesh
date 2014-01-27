#include "EdgeElements.h"

#include <Core/Exception.h>
#include <MeshUtils/Boundary.h>

EdgeElements::EdgeElements(Mesh::Ptr mesh) : m_mesh(mesh) {
    check_mesh();
    extract_boundary_edges();
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

VectorI EdgeElements::getElements() const {
    const size_t num_entries = m_edges.rows() * m_edges.cols();
    VectorI edges(num_entries);
    std::copy(m_edges.data(), m_edges.data() + num_entries, edges.data());
    return edges;
}

VectorI EdgeElements::getElement(size_t ei) const {
    return m_edges.row(ei);
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
