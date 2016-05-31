/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "MeshConnectivity.h"

#include <cassert>
#include <set>
#include <vector>
#include <map>

#include <Mesh.h>
#include <Core/Exception.h>

using namespace PyMesh;

typedef std::set<int> IndexSet;
typedef std::vector<IndexSet> SetArray;

void set_array_to_adj_list(
        SetArray& set_array,
        VectorI& adjacency,
        VectorI& adjacency_idx) {

    size_t num_elements = set_array.size();
    size_t total_adjacency = 0;
    for (size_t i=0; i<num_elements; i++) {
        total_adjacency += set_array[i].size();
    }

    adjacency.resize(total_adjacency);
    adjacency_idx.resize(num_elements + 1);

    size_t count = 0;
    for (size_t i=0; i<num_elements; i++) {
        adjacency_idx[i] = count;
        IndexSet& neighbors = set_array[i];
        for (IndexSet::const_iterator itr = neighbors.begin();
                itr != neighbors.end(); itr++) {
            adjacency[count] = *itr;
            count++;
        }
    }
    adjacency_idx[num_elements] = count;
}

void MeshConnectivity::initialize(Mesh* mesh) {
    init_vertex_adjacencies(mesh);
    init_face_adjacencies(mesh);
    init_voxel_adjacencies(mesh);
}


VectorI MeshConnectivity::get_vertex_adjacent_vertices(size_t vi) const {
    assert(vi <= m_vertex_adjacency_idx.size());
    int pos = m_vertex_adjacency_idx[vi];
    int size = m_vertex_adjacency_idx[vi+1] - pos;
    return m_vertex_adjacency.segment(pos, size);
}

VectorI MeshConnectivity::get_vertex_adjacent_faces(size_t vi) const {
    assert(vi <= m_vertex_face_adjacency_idx.size());
    int pos = m_vertex_face_adjacency_idx[vi];
    int size = m_vertex_face_adjacency_idx[vi+1] - pos;
    return m_vertex_face_adjacency.segment(pos, size);
}

VectorI MeshConnectivity::get_vertex_adjacent_voxels(size_t vi) const {
    assert(vi <= m_vertex_voxel_adjacency_idx.size());
    int pos = m_vertex_voxel_adjacency_idx[vi];
    int size = m_vertex_voxel_adjacency_idx[vi+1] - pos;
    return m_vertex_voxel_adjacency.segment(pos, size);
}

VectorI MeshConnectivity::get_face_adjacent_faces(size_t fi) const {
    assert(fi <= m_face_adjacency_idx.size());
    int pos = m_face_adjacency_idx[fi];
    int size = m_face_adjacency_idx[fi+1] - pos;
    return m_face_adjacency.segment(pos, size);
}

VectorI MeshConnectivity::get_face_adjacent_voxels(size_t fi) const {
    assert(fi <= m_face_voxel_adjacency_idx.size());
    int pos = m_face_voxel_adjacency_idx[fi];
    int size = m_face_voxel_adjacency_idx[fi+1] - pos;
    return m_face_voxel_adjacency.segment(pos, size);
}

VectorI MeshConnectivity::get_voxel_adjacent_faces(size_t Vi) const {
    assert(Vi <= m_voxel_face_adjacency.size());
    int pos = m_voxel_face_adjacency_idx[Vi];
    int size = m_voxel_face_adjacency_idx[Vi+1] - pos;
    return m_voxel_face_adjacency.segment(pos, size);
}

VectorI MeshConnectivity::get_voxel_adjacent_voxels(size_t Vi) const {
    assert(Vi <= m_voxel_adjacency.size());
    int pos = m_voxel_adjacency_idx[Vi];
    int size = m_voxel_adjacency_idx[Vi+1] - pos;
    return m_voxel_adjacency.segment(pos, size);
}


bool MeshConnectivity::vertex_adjacencies_computed() const {
    return m_vertex_adjacency_idx.size() > 0;
}

bool MeshConnectivity::face_adjacencies_computed() const {
    return m_face_adjacency_idx.size() > 0;
}

bool MeshConnectivity::voxel_adjacencies_computed() const {
    return m_voxel_adjacency_idx.size() > 0;
}

void MeshConnectivity::init_vertex_adjacencies(Mesh* mesh) {
    if (vertex_adjacencies_computed()) return;

    const size_t num_vertices = mesh->get_num_vertices();
    const size_t num_faces = mesh->get_num_faces();
    const size_t num_voxels = mesh->get_num_voxels();
    const size_t vertex_per_face = mesh->get_vertex_per_face();
    const size_t vertex_per_voxel = mesh->get_vertex_per_voxel();

    std::vector<IndexSet> neighbor_vertices(num_vertices);
    std::vector<IndexSet> neighbor_faces(num_vertices);
    std::vector<IndexSet> neighbor_voxels(num_vertices);


    if (vertex_per_face == 3) {
        for (size_t i=0; i<num_faces; i++) {
            VectorI face = mesh->get_face(i);
            neighbor_vertices[face[0]].insert(face[1]);
            neighbor_vertices[face[0]].insert(face[2]);
            neighbor_vertices[face[1]].insert(face[0]);
            neighbor_vertices[face[1]].insert(face[2]);
            neighbor_vertices[face[2]].insert(face[0]);
            neighbor_vertices[face[2]].insert(face[1]);

            neighbor_faces[face[0]].insert(i);
            neighbor_faces[face[1]].insert(i);
            neighbor_faces[face[2]].insert(i);
        }
    } else if (vertex_per_face == 4) {
        for (size_t i=0; i<num_faces; i++) {
            VectorI face = mesh->get_face(i);
            neighbor_vertices[face[0]].insert(face[1]);
            neighbor_vertices[face[0]].insert(face[3]);
            neighbor_vertices[face[1]].insert(face[0]);
            neighbor_vertices[face[1]].insert(face[2]);
            neighbor_vertices[face[2]].insert(face[1]);
            neighbor_vertices[face[2]].insert(face[3]);
            neighbor_vertices[face[3]].insert(face[0]);
            neighbor_vertices[face[3]].insert(face[2]);

            neighbor_faces[face[0]].insert(i);
            neighbor_faces[face[1]].insert(i);
            neighbor_faces[face[2]].insert(i);
            neighbor_faces[face[3]].insert(i);
        }
    } else {
        std::stringstream err_msg;
        err_msg << "Unsupported face with " << vertex_per_face
            << " vertices per face";
        throw RuntimeError(err_msg.str());
    }

    if (vertex_per_voxel == 4) {
        for (size_t i=0; i<num_voxels; i++) {
            VectorI voxel = mesh->get_voxel(i);
            neighbor_vertices[voxel[0]].insert(voxel[1]);
            neighbor_vertices[voxel[0]].insert(voxel[2]);
            neighbor_vertices[voxel[0]].insert(voxel[3]);

            neighbor_vertices[voxel[1]].insert(voxel[0]);
            neighbor_vertices[voxel[1]].insert(voxel[2]);
            neighbor_vertices[voxel[1]].insert(voxel[3]);

            neighbor_vertices[voxel[2]].insert(voxel[0]);
            neighbor_vertices[voxel[2]].insert(voxel[1]);
            neighbor_vertices[voxel[2]].insert(voxel[3]);

            neighbor_vertices[voxel[3]].insert(voxel[0]);
            neighbor_vertices[voxel[3]].insert(voxel[1]);
            neighbor_vertices[voxel[3]].insert(voxel[2]);

            neighbor_voxels[voxel[0]].insert(i);
            neighbor_voxels[voxel[1]].insert(i);
            neighbor_voxels[voxel[2]].insert(i);
            neighbor_voxels[voxel[3]].insert(i);
        }
    } else if (vertex_per_voxel == 8) {
        for (size_t i=0; i<num_voxels; i++) {
            VectorI voxel = mesh->get_voxel(i);
            neighbor_vertices[voxel[0]].insert(voxel[1]);
            neighbor_vertices[voxel[0]].insert(voxel[3]);
            neighbor_vertices[voxel[0]].insert(voxel[4]);

            neighbor_vertices[voxel[1]].insert(voxel[0]);
            neighbor_vertices[voxel[1]].insert(voxel[2]);
            neighbor_vertices[voxel[1]].insert(voxel[5]);

            neighbor_vertices[voxel[2]].insert(voxel[1]);
            neighbor_vertices[voxel[2]].insert(voxel[3]);
            neighbor_vertices[voxel[2]].insert(voxel[6]);

            neighbor_vertices[voxel[3]].insert(voxel[0]);
            neighbor_vertices[voxel[3]].insert(voxel[2]);
            neighbor_vertices[voxel[3]].insert(voxel[7]);

            neighbor_vertices[voxel[4]].insert(voxel[0]);
            neighbor_vertices[voxel[4]].insert(voxel[5]);
            neighbor_vertices[voxel[4]].insert(voxel[7]);

            neighbor_vertices[voxel[5]].insert(voxel[1]);
            neighbor_vertices[voxel[5]].insert(voxel[4]);
            neighbor_vertices[voxel[5]].insert(voxel[6]);

            neighbor_vertices[voxel[6]].insert(voxel[2]);
            neighbor_vertices[voxel[6]].insert(voxel[5]);
            neighbor_vertices[voxel[6]].insert(voxel[7]);

            neighbor_vertices[voxel[7]].insert(voxel[3]);
            neighbor_vertices[voxel[7]].insert(voxel[4]);
            neighbor_vertices[voxel[7]].insert(voxel[6]);

            neighbor_voxels[voxel[0]].insert(i);
            neighbor_voxels[voxel[1]].insert(i);
            neighbor_voxels[voxel[2]].insert(i);
            neighbor_voxels[voxel[3]].insert(i);
            neighbor_voxels[voxel[4]].insert(i);
            neighbor_voxels[voxel[5]].insert(i);
            neighbor_voxels[voxel[6]].insert(i);
            neighbor_voxels[voxel[7]].insert(i);
        }
    } else {
        if (num_voxels > 0) {
            std::stringstream err_msg;
            err_msg << "Unsupported voxel with " << vertex_per_voxel
                << " vertices per voxel";
            throw RuntimeError(err_msg.str());
        }
    }

    set_array_to_adj_list(
            neighbor_vertices,
            m_vertex_adjacency,
            m_vertex_adjacency_idx);
    set_array_to_adj_list(
            neighbor_faces,
            m_vertex_face_adjacency,
            m_vertex_face_adjacency_idx);
    set_array_to_adj_list(
            neighbor_voxels,
            m_vertex_voxel_adjacency,
            m_vertex_voxel_adjacency_idx);
}

void MeshConnectivity::init_face_adjacencies(Mesh* mesh) {
    if (face_adjacencies_computed()) return;
    if (!vertex_adjacencies_computed())
        init_vertex_adjacencies(mesh);

    const size_t num_vertices = mesh->get_num_vertices();
    const size_t num_faces = mesh->get_num_faces();
    const size_t num_voxels = mesh->get_num_voxels();
    const size_t vertex_per_face = mesh->get_vertex_per_face();

    std::vector<IndexSet> neighbor_faces(num_faces);
    std::vector<IndexSet> neighbor_voxels(num_faces);

    for (size_t i=0; i<num_faces; i++) {
        VectorI face = mesh->get_face(i);
        IndexSet& face_neighbors = neighbor_faces[i];

        assert(face.size() == vertex_per_face);
        std::map<int, int> face_counter;
        for (size_t j=0; j<vertex_per_face; j++) {
            VectorI neighbors = get_vertex_adjacent_faces(face[j]);
            size_t num_neighbors = neighbors.size();
            for (size_t k=0; k<num_neighbors; k++) {
                if (face_counter.find(neighbors[k]) == face_counter.end()) {
                    face_counter[neighbors[k]] = 1;
                } else {
                    face_counter[neighbors[k]] += 1;
                }
            }
        }
        for (std::map<int, int>::const_iterator itr = face_counter.begin();
                itr != face_counter.end(); itr++) {
            if (itr->second == 2) {
                face_neighbors.insert(itr->first);
            }
        }
    }

    for (size_t i=0; i<num_faces; i++) {
        VectorI face = mesh->get_face(i);
        IndexSet& face_neighbors = neighbor_voxels[i];

        std::map<int, int> voxel_counter;
        for (size_t j=0; j<vertex_per_face; j++) {
            VectorI neighbors = get_vertex_adjacent_voxels(face[j]);
            size_t num_neighbors = neighbors.size();
            for (size_t k=0; k<num_neighbors; k++) {
                if (voxel_counter.find(neighbors[k]) == voxel_counter.end()) {
                    voxel_counter[neighbors[k]] = 1;
                } else {
                    voxel_counter[neighbors[k]] += 1;
                }
            }
        }
        for (std::map<int, int>::const_iterator itr = voxel_counter.begin();
                itr != voxel_counter.end(); itr++) {
            if (itr->second == 3) {
                face_neighbors.insert(itr->first);
            }
        }
    }

    set_array_to_adj_list(
            neighbor_faces,
            m_face_adjacency,
            m_face_adjacency_idx);
    set_array_to_adj_list(
            neighbor_voxels,
            m_face_voxel_adjacency,
            m_face_voxel_adjacency_idx);
}

void MeshConnectivity::init_voxel_adjacencies(Mesh* mesh) {
    if (voxel_adjacencies_computed()) return;
    if (!vertex_adjacencies_computed())
        init_vertex_adjacencies(mesh);

    size_t num_vertices = mesh->get_num_vertices();
    size_t num_faces = mesh->get_num_faces();
    size_t num_voxels = mesh->get_num_voxels();
    size_t vertex_per_voxel = mesh->get_vertex_per_voxel();

    std::vector<IndexSet> neighbor_faces(num_voxels);
    std::vector<IndexSet> neighbor_voxels(num_voxels);

    for (size_t i=0; i<num_voxels; i++) {
        VectorI voxel = mesh->get_voxel(i);
        IndexSet& voxel_neighbors = neighbor_faces[i];

        assert(voxel.size() == vertex_per_voxel);
        std::map<int, int> face_counter;
        for (size_t j=0; j<vertex_per_voxel; j++) {
            VectorI neighbors = get_vertex_adjacent_faces(voxel[j]);
            size_t num_neighbors = neighbors.size();
            for (size_t k=0; k<num_neighbors; k++) {
                if (face_counter.find(neighbors[k]) == face_counter.end()) {
                    face_counter[neighbors[k]] = 1;
                } else {
                    face_counter[neighbors[k]] += 1;
                }
            }
        }
        for (std::map<int, int>::const_iterator itr = face_counter.begin();
                itr != face_counter.end(); itr++) {
            if (itr->second == 3) {
                voxel_neighbors.insert(itr->first);
            }
        }
    }

    for (size_t i=0; i<num_voxels; i++) {
        VectorI voxel = mesh->get_voxel(i);
        IndexSet& voxel_neighbors = neighbor_voxels[i];

        std::map<int, int> voxel_counter;
        for (size_t j=0; j<vertex_per_voxel; j++) {
            VectorI neighbors = get_vertex_adjacent_voxels(voxel[j]);
            size_t num_neighbors = neighbors.size();
            for (size_t k=0; k<num_neighbors; k++) {
                if (voxel_counter.find(neighbors[k]) == voxel_counter.end()) {
                    voxel_counter[neighbors[k]] = 1;
                } else {
                    voxel_counter[neighbors[k]] += 1;
                }
            }
        }
        for (std::map<int, int>::const_iterator itr = voxel_counter.begin();
                itr != voxel_counter.end(); itr++) {
            if (itr->second == 4) {
                voxel_neighbors.insert(itr->first);
            }
        }
    }

    set_array_to_adj_list(
            neighbor_voxels,
            m_voxel_adjacency,
            m_voxel_adjacency_idx);
    set_array_to_adj_list(
            neighbor_faces,
            m_voxel_face_adjacency,
            m_voxel_face_adjacency_idx);
}

void MeshConnectivity::clear() {
    m_vertex_adjacency.resize(0);
    m_vertex_adjacency_idx.resize(0);
    m_vertex_face_adjacency.resize(0);
    m_vertex_face_adjacency_idx.resize(0);
    m_vertex_voxel_adjacency.resize(0);
    m_vertex_voxel_adjacency_idx.resize(0);

    m_face_adjacency.resize(0);
    m_face_adjacency_idx.resize(0);
    m_face_voxel_adjacency.resize(0);
    m_face_voxel_adjacency_idx.resize(0);

    m_voxel_adjacency.resize(0);
    m_voxel_adjacency_idx.resize(0);
    m_voxel_face_adjacency.resize(0);
    m_voxel_face_adjacency_idx.resize(0);
}

