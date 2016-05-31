/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "MeshSeparator.h"

#include <cassert>
#include <list>
#include <queue>

#include <Core/EigenTypedef.h>
#include <Core/Exception.h>
#include <Misc/Multiplet.h>
#include <Misc/MultipletMap.h>

using namespace PyMesh;

MeshSeparator::MeshSeparator(const MatrixI& elements)
    : m_elements(elements), m_connectivity_type(VERTEX) { }

size_t MeshSeparator::separate() {
    compute_connectivity();

    m_components.clear();
    m_sources.clear();
    const size_t num_elements = m_elements.rows();
    m_visited = std::vector<bool>(num_elements, false);

    for (size_t i=0; i<num_elements; i++) {
        if (m_visited[i]) continue;
        VectorI sources;
        MatrixI comp_f = flood(i, sources);
        m_components.push_back(comp_f);
        m_sources.push_back(sources);
    }

    return m_components.size();
}

void MeshSeparator::compute_connectivity() {
    m_connectivity.clear();
    switch(m_connectivity_type) {
        case VERTEX:
            compute_vertex_connectivity();
            break;
        case FACE:
            compute_face_connectivity();
            break;
        case VOXEL:
            compute_voxel_connectivity();
            break;
    }
}

void MeshSeparator::compute_vertex_connectivity() {
    const size_t num_elements = m_elements.rows();
    const size_t vertex_per_element = m_elements.cols();
    for (size_t i=0; i<num_elements; i++) {
        const auto& elment = m_elements.row(i);
        for (size_t j=0; j<vertex_per_element; j++) {
            m_connectivity.insert(Connector(m_elements(i, j)), i);
        }
    }
}

void MeshSeparator::compute_face_connectivity() {
    const size_t num_elements = m_elements.rows();
    const size_t vertex_per_element = m_elements.cols();
    if (vertex_per_element != 3 && vertex_per_element != 4) {
        throw RuntimeError(
                "Unknow face type!  Only triangle and quad faces are supported");
    }

    for (size_t i=0; i<num_elements; i++) {
        const auto& e = m_elements.row(i);
        for (size_t j=0; j<vertex_per_element; j++) {
            m_connectivity.insert(Connector(e[j], e[(j+1)%vertex_per_element]), i);
        }
    }
}

void MeshSeparator::compute_voxel_connectivity() {
    const size_t num_elements= m_elements.rows();
    const size_t vertex_per_element = m_elements.cols();
    if (vertex_per_element == 4) {
        for (size_t i=0; i<num_elements; i++) {
            const auto& voxel = m_elements.row(i);
            m_connectivity.insert(Connector(voxel[0], voxel[1], voxel[2]), i);
            m_connectivity.insert(Connector(voxel[1], voxel[2], voxel[3]), i);
            m_connectivity.insert(Connector(voxel[2], voxel[3], voxel[0]), i);
            m_connectivity.insert(Connector(voxel[3], voxel[0], voxel[1]), i);
        }
    } else if (vertex_per_element == 8) {
        for (size_t i=0; i<num_elements; i++) {
            const auto& voxel = m_elements.row(i);
            m_connectivity.insert(
                    Connector(voxel[0], voxel[1], voxel[2], voxel[3]), i);
            m_connectivity.insert(
                    Connector(voxel[4], voxel[5], voxel[6], voxel[7]), i);
            m_connectivity.insert(
                    Connector(voxel[0], voxel[4], voxel[7], voxel[3]), i);
            m_connectivity.insert(
                    Connector(voxel[1], voxel[5], voxel[6], voxel[2]), i);
            m_connectivity.insert(
                    Connector(voxel[0], voxel[1], voxel[4], voxel[5]), i);
            m_connectivity.insert(
                    Connector(voxel[3], voxel[2], voxel[6], voxel[7]), i);
        }
    } else {
        throw RuntimeError(
                "Only tetrahedron and hexahedron elements are supported");
    }
}

MatrixI MeshSeparator::flood(size_t seed, VectorI& sources) {
    std::queue<size_t> Q;
    Q.push(seed);
    m_visited[seed] = true;

    std::list<size_t> element_list;
    while (!Q.empty()) {
        size_t ei = Q.front();
        Q.pop();
        element_list.push_back(ei);

        std::vector<size_t> neighbors = get_adjacent_element(ei);
        for (size_t i=0; i<neighbors.size(); i++) {
            size_t f_next = neighbors[i];
            if (m_visited[f_next]) continue;
            Q.push(f_next);
            m_visited[f_next] = true;
        }
    }

    const size_t vertex_per_element = m_elements.cols();
    MatrixI comp(element_list.size(), vertex_per_element);
    sources.resize(element_list.size());
    size_t count = 0;
    for (std::list<size_t>::const_iterator itr = element_list.begin();
            itr != element_list.end(); itr++) {
        assert(count < m_elements.rows());
        assert(*itr < m_elements.rows());
        comp.row(count) = m_elements.row(*itr);
        sources[count] = *itr;
        count++;
    }
    return comp;
}

std::vector<size_t> MeshSeparator::get_adjacent_element(size_t index) {
    std::vector<size_t> neighbors;
    const auto& element = m_elements.row(index);
    const size_t vertex_per_element = element.size();

    switch (m_connectivity_type) {
        case VERTEX:
            for (size_t i=0; i<vertex_per_element; i++) {
                Connector conn(element[i]);
                AdjElements& neighbor = m_connectivity[conn];
                neighbors.insert(neighbors.end(), neighbor.begin(), neighbor.end());
            }
            break;
        case FACE:
            for (size_t i=0; i<vertex_per_element; i++) {
                Connector conn(element[i], element[(i+1)%vertex_per_element]);
                AdjElements& neighbor = m_connectivity[conn];
                neighbors.insert(neighbors.end(), neighbor.begin(), neighbor.end());
            }
            break;
        case VOXEL:
            if (vertex_per_element == 4) {
                for (size_t i=0; i<vertex_per_element; i++) {
                    Connector conn(element[i],
                            element[(i+1)%vertex_per_element],
                            element[(i+2)%vertex_per_element]);
                    AdjElements& neighbor = m_connectivity[conn];
                    neighbors.insert(neighbors.end(), neighbor.begin(), neighbor.end());
                }
            } else if (vertex_per_element == 8) {
                Connector connectors[] = {
                    Connector(element[0], element[1], element[2], element[3]),
                    Connector(element[4], element[5], element[6], element[7]),
                    Connector(element[0], element[4], element[7], element[3]),
                    Connector(element[1], element[5], element[6], element[2]),
                    Connector(element[0], element[1], element[4], element[5]),
                    Connector(element[3], element[2], element[6], element[7])
                };
                for (size_t i=0; i<6; i++) {
                    AdjElements& neighbor = m_connectivity[connectors[i]];
                    neighbors.insert(neighbors.end(), neighbor.begin(), neighbor.end());
                }
            } else {
                throw RuntimeError("Only tet and hex elements are supported.");
            }
            break;
    }

    return neighbors;
}

void MeshSeparator::clear() {
    m_components.clear();
    m_sources.clear();
    m_visited.clear();
    m_connectivity.clear();
}
