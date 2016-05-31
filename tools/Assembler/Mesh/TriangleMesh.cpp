/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "TriangleMesh.h"

#include <iostream>
#include <algorithm>
#include <cassert>
#include <set>

#include <Mesh.h>
#include <Core/Exception.h>

using namespace PyMesh;

TriangleMesh::TriangleMesh(TriangleMesh::MeshPtr mesh) : m_mesh(mesh) {
    verify_triangle_mesh();
    init_boundary();
}

size_t TriangleMesh::getDim() const {
    return m_mesh->get_dim();
}

size_t TriangleMesh::getNbrNodes() const {
    return m_mesh->get_num_vertices();
}

VectorF TriangleMesh::getNodes() const {
    return m_mesh->get_vertices();
}

const VectorF& TriangleMesh::getNodesRaw() const {
    return m_mesh->get_vertices();
}

VectorF TriangleMesh::getNode(size_t vi) const {
    assert(vi < getNbrNodes());
    return m_mesh->get_vertex(vi);
}

VectorI TriangleMesh::getNodeAdjacentNodes(size_t vi) const {
    assert(vi < getNbrNodes());
    return m_mesh->get_vertex_adjacent_vertices(vi);
}

VectorI TriangleMesh::getNodeAdjacentElements(size_t vi) const {
    assert(vi < getNbrNodes());
    return m_mesh->get_vertex_adjacent_faces(vi);
}

bool TriangleMesh::isBoundaryNode(size_t vi) const {
    assert(vi < getNbrNodes());
    return m_boundary_idx[vi] != -1;
}

size_t TriangleMesh::getNbrElements() const {
    return m_mesh->get_num_faces();
}

size_t TriangleMesh::getNodePerElement() const {
    assert(m_mesh->get_vertex_per_face() == 3);
    return m_mesh->get_vertex_per_face();
}

VectorI TriangleMesh::getElements() const {
    return m_mesh->get_faces();
}

const VectorI& TriangleMesh::getElementsRaw() const {
    return m_mesh->get_faces();
}

VectorI TriangleMesh::getElement(size_t ei) const {
    assert(ei < getNbrElements());
    return m_mesh->get_face(ei);
}

VectorI TriangleMesh::getElementAdjacentElements(size_t ei) const {
    assert(ei < getNbrElements());
    return m_mesh->get_face_adjacent_faces(ei);
}

Float TriangleMesh::getElementVolume(size_t ei) const {
    assert(ei < getNbrElements());
    return m_mesh->get_attribute("face_area")[ei];
}

bool TriangleMesh::isBoundaryElement(size_t ei) const {
    assert(ei < getNbrElements());
    VectorI elem = getElement(ei);
    assert(3 == elem.size());
    return isBoundaryNode(elem[0]) ||
           isBoundaryNode(elem[1]) ||
           isBoundaryNode(elem[2]);
}

size_t TriangleMesh::getNbrBoundaryFaces() const {
    return m_boundary_edges.rows();
}

size_t TriangleMesh::getNodePerBoundaryFace() const {
    return 2;
}

VectorI TriangleMesh::getBoundaryFace(size_t bfi) const {
    assert(bfi < getNbrBoundaryFaces());
    return m_boundary_edges.row(bfi);
}

Float TriangleMesh::getBoundaryFaceArea(size_t bfi) const {
    assert(bfi < getNbrBoundaryFaces());
    return m_boundary_lengths(bfi);
}

VectorF TriangleMesh::getBoundaryFaceNormal(size_t bfi) const {
    assert(bfi < getNbrBoundaryFaces());
    return m_boundary_normals.row(bfi);
}

size_t TriangleMesh::getNbrBoundaryNodes() const {
    return m_boundary.size();
}

VectorI TriangleMesh::getBoundaryNodes() const {
    return m_boundary;
}

const VectorI& TriangleMesh::getBoundaryNodesRaw() const {
    return m_boundary;
}

size_t TriangleMesh::getBoundaryNode(size_t bvi) const {
    assert(bvi < getNbrBoundaryNodes());
    return m_boundary[bvi];
}

size_t TriangleMesh::getBoundaryIndex(size_t vi) const {
    assert(vi < getNbrNodes());
    return m_boundary_idx[vi];
}

VectorF TriangleMesh::getBoundaryNodeNormal(size_t bvi) const {
    assert(bvi < getNbrBoundaryNodes());
    return m_boundary_vertex_normals.row(bvi);
}

VectorI TriangleMesh::getBoundaryNodeAdjacentBoundaryFaces(size_t bvi) const {
    assert(bvi < getNbrBoundaryNodes());
    size_t begin = m_boundary_node_edge_adj_idx[bvi];
    size_t end   = m_boundary_node_edge_adj_idx[bvi+1];
    return m_boundary_node_edge_adj.segment(begin, end-begin);
}

VectorI TriangleMesh::getBoundaryNodeAdjacentBoundaryNodes(size_t bvi) const {
    assert(bvi < getNbrBoundaryNodes());
    size_t begin = m_boundary_node_adj_idx[bvi];
    size_t end   = m_boundary_node_adj_idx[bvi+1];
    return m_boundary_node_adj.segment(begin, end-begin);
}

void TriangleMesh::verify_triangle_mesh() {
    const size_t vertex_per_face = m_mesh->get_vertex_per_face();
    if (vertex_per_face != 3) {
        throw RuntimeError("Not a triangule mesh");
    }
}

void TriangleMesh::init_boundary() {
    BoundaryPtr bd = Boundary::extract_surface_boundary(*m_mesh.get());
    init_boundary_nodes(bd);
    init_boundary_edges(bd);
    init_boundary_node_adjacencies();
    init_boundary_face_adjacencies();
    init_boundary_lengths();
    init_boundary_normals(bd);
    init_boundary_vertex_normals();
}

void TriangleMesh::init_boundary_nodes(const BoundaryPtr& bd) {
    const size_t num_vertices = m_mesh->get_num_vertices();
    const size_t num_boundaries = bd->get_num_boundaries();
    std::set<size_t> boundary_vertices;
    for (size_t i=0; i<num_boundaries; i++) {
        VectorI bd_edge = bd->get_boundary(i);
        boundary_vertices.insert(bd_edge.data(), bd_edge.data() + 2);
    }

    m_boundary.resize(boundary_vertices.size());
    std::copy(boundary_vertices.begin(), boundary_vertices.end(),
            m_boundary.data());

    m_boundary_idx = VectorI::Ones(num_vertices);
    m_boundary_idx *= -1;
    const size_t num_boundary_vertices = m_boundary.size();
    for (size_t i=0; i<num_boundary_vertices; i++) {
        m_boundary_idx[m_boundary[i]] = i;
    }
}

void TriangleMesh::init_boundary_edges(const BoundaryPtr& bd) {
    m_boundary_edges = bd->get_boundaries();
}

void TriangleMesh::init_boundary_node_adjacencies() {
    const size_t num_boundary_edges = m_boundary_edges.rows();
    const size_t num_boundary_vertices = m_boundary.size();
    std::vector<std::set<size_t> > bd_vertex_adjacencies(num_boundary_vertices);
    for (size_t i=0; i<num_boundary_edges; i++) {
        const VectorI& edge = m_boundary_edges.row(i);
        bd_vertex_adjacencies[edge[0]].insert(edge[1]);
        bd_vertex_adjacencies[edge[1]].insert(edge[0]);
    }

    std::vector<size_t> adj;
    std::vector<size_t> adj_idx;
    adj_idx.push_back(0);
    for (size_t i=0; i<num_boundary_vertices; i++) {
        const std::set<size_t>& neighbors = bd_vertex_adjacencies[i];
        adj.insert(adj.end(), neighbors.begin(), neighbors.end());
        adj_idx.push_back(adj.size());
    }

    m_boundary_node_adj.resize(adj.size());
    std::copy(adj.begin(), adj.end(), m_boundary_node_adj.data());

    m_boundary_node_adj_idx.resize(adj_idx.size());
    std::copy(adj_idx.begin(), adj_idx.end(), m_boundary_node_adj_idx.data());
}

void TriangleMesh::init_boundary_face_adjacencies() {
    const size_t num_boundary_edges = m_boundary_edges.rows();
    const size_t num_boundary_vertices = m_boundary.size();
    std::vector<std::set<size_t> > bd_vertex_edge_adjacencies(num_boundary_vertices);
    for (size_t i=0; i<num_boundary_edges; i++) {
        const VectorI& edge = m_boundary_edges.row(i);
        bd_vertex_edge_adjacencies[edge[0]].insert(i);
        bd_vertex_edge_adjacencies[edge[1]].insert(i);
    }

    std::vector<size_t> adj;
    std::vector<size_t> adj_idx;
    adj_idx.push_back(0);
    for (size_t i=0; i<num_boundary_vertices; i++) {
        const std::set<size_t>& neighbors = bd_vertex_edge_adjacencies[i];
        adj.insert(adj.end(), neighbors.begin(), neighbors.end());
        adj_idx.push_back(adj.size());
    }

    m_boundary_node_edge_adj.resize(adj.size());
    std::copy(adj.begin(), adj.end(), m_boundary_node_edge_adj.data());

    m_boundary_node_edge_adj_idx.resize(adj_idx.size());
    std::copy(adj_idx.begin(), adj_idx.end(), m_boundary_node_edge_adj_idx.data());
}

void TriangleMesh::init_boundary_lengths() {
    const size_t num_edges = m_boundary_edges.rows();
    m_boundary_lengths.resize(num_edges);
    for (size_t i=0; i<num_edges; i++) {
        const VectorI& edge = m_boundary_edges.row(i);
        assert(2 == edge.size());
        VectorF v0 = m_mesh->get_vertex(edge[0]);
        VectorF v1 = m_mesh->get_vertex(edge[1]);
        m_boundary_lengths[i] = (v0 - v1).norm();
    }
}

void TriangleMesh::init_boundary_normals(const BoundaryPtr& bd) {
    const size_t dim = m_mesh->get_dim();
    const size_t num_edges = m_boundary_edges.rows();
    m_boundary_normals.resize(num_edges, 3);
    for (size_t i=0; i<num_edges; i++) {
        const VectorI& edge = m_boundary_edges.row(i);
        assert(2 == edge.size());
        VectorF v0 = m_mesh->get_vertex(edge[0]);
        VectorF v1 = m_mesh->get_vertex(edge[1]);

        size_t face_idx = bd->get_boundary_element(i);
        VectorF normal = m_mesh->get_attribute("face_normal").segment(
                face_idx * 3, 3);

        Vector3F edge_v, normal_v;
        edge_v.segment(0, dim) = v1 - v0;
        normal_v.segment(0, 3) = normal;

        Vector3F edge_normal = edge_v.cross(normal_v).normalized();
        m_boundary_normals.row(i) = edge_normal;
    }
}

void TriangleMesh::init_boundary_vertex_normals() {
    const size_t dim = getDim();
    const size_t num_bd_vertices = getNbrBoundaryNodes();
    m_boundary_vertex_normals.resize(num_bd_vertices, 3);
    for (size_t bvi=0; bvi<num_bd_vertices; bvi++) {
        VectorI adj_edges = getBoundaryNodeAdjacentBoundaryFaces(bvi);
        VectorF normal = VectorF::Zero(3);
        const size_t num_adj_edges = adj_edges.size();
        for (size_t j=0; j<num_adj_edges; j++) {
            size_t bej = adj_edges[j];
            VectorF edge_normal = getBoundaryFaceNormal(bej);
            Float edge_len = getBoundaryFaceArea(bej);
            normal += edge_normal * edge_len;
        }
        normal.normalize();
        m_boundary_vertex_normals.row(bvi) = normal;
    }
}

