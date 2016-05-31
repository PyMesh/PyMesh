/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "TetrahedronMesh.h"
#include <algorithm>
#include <cassert>
#include <vector>

#include <Mesh.h>
#include <Core/Exception.h>

using namespace PyMesh;

TetrahedronMesh::TetrahedronMesh(TetrahedronMesh::MeshPtr mesh) : m_mesh(mesh) {
    verify_voxel_mesh();
    init_attributes();
    init_boundary_nodes();
    init_boundary_node_adjacencies();
}

size_t TetrahedronMesh::getDim() const {
    return m_mesh->get_dim();
}

size_t TetrahedronMesh::getNbrNodes() const {
    return m_mesh->get_num_vertices();
}

VectorF TetrahedronMesh::getNodes() const {
    return m_mesh->get_vertices();
}

const VectorF& TetrahedronMesh::getNodesRaw() const {
    return m_mesh->get_vertices();
}

VectorF TetrahedronMesh::getNode(size_t vi) const {
    assert(vi < getNbrNodes());
    return m_mesh->get_vertex(vi);
}

VectorI TetrahedronMesh::getNodeAdjacentNodes(size_t vi) const {
    assert(vi < getNbrNodes());
    return m_mesh->get_vertex_adjacent_vertices(vi);
}

VectorI TetrahedronMesh::getNodeAdjacentElements(size_t vi) const {
    assert(vi < getNbrNodes());
    return m_mesh->get_vertex_adjacent_voxels(vi);
}

bool TetrahedronMesh::isBoundaryNode(size_t vi) const {
    assert(vi < getNbrNodes());
    return m_boundary_idx[vi] != -1;
}

size_t TetrahedronMesh::getNbrElements() const {
    return m_mesh->get_num_voxels();
}

size_t TetrahedronMesh::getNodePerElement() const {
    assert(m_mesh->get_vertex_per_voxel() == 4);
    return m_mesh->get_vertex_per_voxel();
}

VectorI TetrahedronMesh::getElements() const {
    return m_mesh->get_voxels();
}

const VectorI& TetrahedronMesh::getElementsRaw() const {
    return m_mesh->get_voxels();
}

VectorI TetrahedronMesh::getElement(size_t ei) const {
    assert(ei < getNbrElements());
    return m_mesh->get_voxel(ei);
}

VectorI TetrahedronMesh::getElementAdjacentElements(size_t ei) const {
    assert(ei < getNbrElements());
    return m_mesh->get_voxel_adjacent_voxels(ei);
}

Float TetrahedronMesh::getElementVolume(size_t ei) const {
    assert(ei < getNbrElements());
    return m_mesh->get_attribute("voxel_volume")[ei];
}

bool TetrahedronMesh::isBoundaryElement(size_t ei) const {
    assert(ei < getNbrElements());
    VectorI elem = getElement(ei);
    assert(4 == elem.size());
    return isBoundaryNode(elem[0]) ||
           isBoundaryNode(elem[1]) ||
           isBoundaryNode(elem[2]) ||
           isBoundaryNode(elem[3]);
}

size_t TetrahedronMesh::getNbrBoundaryFaces() const {
    return m_mesh->get_num_faces();
}

size_t TetrahedronMesh::getNodePerBoundaryFace() const {
    assert(m_mesh->get_vertex_per_face() == 3);
    return m_mesh->get_vertex_per_face();
}

VectorI TetrahedronMesh::getBoundaryFace(size_t bfi) const {
    assert(bfi < getNbrBoundaryFaces());
    return m_mesh->get_face(bfi);
}

Float TetrahedronMesh::getBoundaryFaceArea(size_t bfi) const {
    assert(bfi < getNbrBoundaryFaces());
    return m_mesh->get_attribute("face_area")[bfi];
}

VectorF TetrahedronMesh::getBoundaryFaceNormal(size_t bfi) const {
    assert(bfi < getNbrBoundaryFaces());
    return m_mesh->get_attribute("face_normal").segment(3*bfi, 3);
}

size_t TetrahedronMesh::getNbrBoundaryNodes() const {
    return m_boundary.size();
}

VectorI TetrahedronMesh::getBoundaryNodes() const {
    return m_boundary;
}

const VectorI& TetrahedronMesh::getBoundaryNodesRaw() const {
    return m_boundary;
}

size_t TetrahedronMesh::getBoundaryNode(size_t bvi) const {
    assert(bvi < getNbrBoundaryNodes());
    return m_boundary[bvi];
}

size_t TetrahedronMesh::getBoundaryIndex(size_t vi) const {
    assert(vi < m_boundary_idx.size());
    return m_boundary_idx[vi];
}

VectorF TetrahedronMesh::getBoundaryNodeNormal(size_t bvi) const {
    assert(bvi < getNbrBoundaryNodes());
    size_t vi = getBoundaryNode(bvi);
    return m_mesh->get_attribute("vertex_normal").segment(vi*3, 3);
}

VectorI TetrahedronMesh::getBoundaryNodeAdjacentBoundaryFaces(size_t bvi) const {
    assert(bvi < getNbrBoundaryNodes());
    size_t vi = getBoundaryNode(bvi);
    return m_mesh->get_vertex_adjacent_faces(vi);
}

VectorI TetrahedronMesh::getBoundaryNodeAdjacentBoundaryNodes(size_t bvi) const {
    assert(bvi < getNbrBoundaryNodes());
    size_t begin = m_bd_node_adj_idx[bvi];
    size_t end = m_bd_node_adj_idx[bvi + 1];
    return m_bd_node_adj.segment(begin, end-begin);
}

void TetrahedronMesh::verify_voxel_mesh() {
    const size_t dim = m_mesh->get_dim();
    if (dim != 3) {
        throw RuntimeError("Not a 3 dimensional mesh.");
    }
    const size_t num_voxels = m_mesh->get_num_voxels();
    if (num_voxels == 0) {
        throw RuntimeError("Mesh contains 0 voxels.");
    }
    const size_t vertex_per_voxel = m_mesh->get_vertex_per_voxel();
    if (vertex_per_voxel != 4) {
        throw RuntimeError("Not a tetrahedron mesh.");
    }
}

void TetrahedronMesh::init_attributes() {
    if (!m_mesh->has_attribute("vertex_normal")) {
        throw RuntimeError("\"vertex_normal\" is not computed in mesh");
    }
    if (!m_mesh->has_attribute("voxel_volume")) {
        throw RuntimeError("\"voxel_volume\" is not computed in mesh");
    }
}

void TetrahedronMesh::init_boundary_nodes() {
    const size_t dim = getDim();
    const size_t num_faces = m_mesh->get_num_faces();
    const size_t num_vertices = m_mesh->get_num_vertices();
    const size_t vertex_per_face = m_mesh->get_vertex_per_face();
    const VectorI& faces = m_mesh->get_faces();

    size_t count = 1;
    m_boundary_idx = VectorI::Zero(num_vertices);
    for (size_t i=0; i<num_faces * vertex_per_face; i++) {
        if (m_boundary_idx[faces[i]] == 0) {
            m_boundary_idx[faces[i]] = count;
            count ++;
        }
    }
    m_boundary_idx.array() -= 1;

    m_boundary.resize(count-1);
    for (size_t i=0; i<num_vertices; i++) {
        if (m_boundary_idx[i] >= 0) {
            m_boundary[m_boundary_idx[i]] = i;
        }
    }
}

void TetrahedronMesh::init_boundary_node_adjacencies() {
    std::vector<size_t> bv_adj;
    std::vector<size_t> bv_adj_idx;

    const size_t num_boundary_nodes = getNbrBoundaryNodes();
    for (size_t i=0; i<num_boundary_nodes; i++) {
        bv_adj_idx.push_back(bv_adj.size());
        size_t vi = getBoundaryNode(i);
        const VectorI neighbors = getNodeAdjacentNodes(vi);
        for (size_t j=0; j<neighbors.size(); j++) {
            size_t vj = neighbors[j];
            if (!isBoundaryNode(vj)) continue;
            bv_adj.push_back(getBoundaryIndex(vj));
        }
    }
    bv_adj_idx.push_back(bv_adj.size());

    m_bd_node_adj.resize(bv_adj.size());
    m_bd_node_adj_idx.resize(bv_adj_idx.size());
    std::copy(bv_adj.begin(), bv_adj.end(), m_bd_node_adj.data());
    std::copy(bv_adj_idx.begin(), bv_adj_idx.end(), m_bd_node_adj_idx.data());
}


