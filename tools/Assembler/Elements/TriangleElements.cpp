#include "TriangleElements.h"

#include <Core/Exception.h>

TriangleElements::TriangleElements(Mesh::Ptr mesh) : m_mesh(mesh) {
    check_mesh();
}

size_t TriangleElements::getDim() const {
    return m_mesh->get_dim();
}

size_t TriangleElements::getNbrNodes() const {
    return m_mesh->get_num_vertices();
}

VectorF TriangleElements::getNodes() const {
    return m_mesh->get_vertices();
}

VectorF TriangleElements::getNode(size_t vi) const {
    return m_mesh->get_vertex(vi);
}

size_t TriangleElements::getNbrElements() const {
    return m_mesh->get_num_faces();
}

size_t TriangleElements::getNodePerElement() const {
    return 3;
}

VectorI TriangleElements::getElements() const {
    return m_mesh->get_faces();
}

VectorI TriangleElements::getElement(size_t ei) const {
    return m_mesh->get_face(ei);
}

Float TriangleElements::getElementVolume(size_t ei) const {
    return m_mesh->get_attribute("face_area")[ei];
}

void TriangleElements::check_mesh() {
    if (m_mesh->get_num_faces() == 0) {
        throw RuntimeError("Mesh contains zero faces.");
    }
    if (m_mesh->get_vertex_per_face() != 3) {
        throw RuntimeError("Mesh is not a triangle mesh.");
    }
}
