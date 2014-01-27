#include "TetrahedronElements.h"

#include <Core/Exception.h>

TetrahedronElements::TetrahedronElements(Mesh::Ptr mesh) : m_mesh(mesh) {
    check_mesh();
}

size_t TetrahedronElements::getDim() const {
    return m_mesh->get_dim();
}

size_t TetrahedronElements::getNbrNodes() const {
    return m_mesh->get_num_vertices();
}

VectorF TetrahedronElements::getNodes() const {
    return m_mesh->get_vertices();
}

VectorF TetrahedronElements::getNode(size_t vi) const {
    return m_mesh->get_vertex(vi);
}

size_t TetrahedronElements::getNbrElements() const {
    return m_mesh->get_num_voxels();
}

VectorI TetrahedronElements::getElements() const {
    return m_mesh->get_voxels();
}

VectorI TetrahedronElements::getElement(size_t ei) const {
    return m_mesh->get_voxel(ei);
}

void TetrahedronElements::check_mesh() {
    if (m_mesh->get_dim() != 3) {
        throw RuntimeError("Tetrahedron mesh must be in R^3.");
    }
    if (m_mesh->get_num_voxels() == 0) {
        throw RuntimeError("Mesh contains zero elements.");
    }
    if (m_mesh->get_vertex_per_voxel() != 4) {
        throw RuntimeError("Mesh is not a tetrahedron mesh.");
    }
}
