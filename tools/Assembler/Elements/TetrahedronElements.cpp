/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "TetrahedronElements.h"

#include <Core/Exception.h>

using namespace PyMesh;

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

size_t TetrahedronElements::getNodePerElement() const {
    return 4;
}

VectorI TetrahedronElements::getElements() const {
    return m_mesh->get_voxels();
}

VectorI TetrahedronElements::getElement(size_t ei) const {
    return m_mesh->get_voxel(ei);
}

Float TetrahedronElements::getElementVolume(size_t ei) const {
    return m_mesh->get_attribute("voxel_volume")[ei];
}

VectorF TetrahedronElements::getElementCenter(size_t ei) const {
    VectorI elem = m_mesh->get_voxel(ei);
    VectorF v1 = m_mesh->get_vertex(elem[0]);
    VectorF v2 = m_mesh->get_vertex(elem[1]);
    VectorF v3 = m_mesh->get_vertex(elem[2]);
    VectorF v4 = m_mesh->get_vertex(elem[3]);
    return 0.25 * (v1 + v2 + v3 + v4);
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
    if (!m_mesh->has_attribute("voxel_volume")) {
        m_mesh->add_attribute("voxel_volume");
    }
}
