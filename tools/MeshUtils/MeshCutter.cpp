/* This file is part of PyMesh. Copyright (c) 2018 by Qingnan Zhou */
#include "MeshCutter.h"

#include <cassert>
#include <iostream>
#include <unordered_map>
#include <MeshFactory.h>
#include <Core/Exception.h>

using namespace PyMesh;

MeshCutter::MeshCutter(Mesh::Ptr mesh) : m_mesh(mesh) {
    m_mesh->enable_vertex_connectivity();
}

Mesh::Ptr MeshCutter::cut_with_face_labels(const std::vector<size_t>& comp_ids) const {
    const size_t num_faces = m_mesh->get_num_faces();
    if (comp_ids.size() != num_faces) {
        throw RuntimeError("Comp ids size does not match the number of faces in mesh");
    }
    const size_t dim = m_mesh->get_dim();
    const size_t num_vertices = m_mesh->get_num_vertices();
    const size_t vertex_per_face = m_mesh->get_vertex_per_face();
    const auto& vertices = m_mesh->get_vertices();
    const auto& faces = m_mesh->get_faces();

    std::vector<size_t> added_vertices;
    std::vector<std::unordered_map<size_t, size_t>> index_map(num_vertices);
    for (size_t i=0; i<num_vertices; i++) {
        const auto& adj_faces = m_mesh->get_vertex_adjacent_faces(i);
        const size_t num_adj_faces = adj_faces.size();
        if (num_adj_faces == 0) continue;

        bool is_interior_vtx = true;
        for (size_t j=1; is_interior_vtx && j<num_adj_faces; j++) {
            is_interior_vtx &= (comp_ids[adj_faces[0]] == comp_ids[adj_faces[j]]);
        }
        if (is_interior_vtx) continue;

        auto min_id = comp_ids[adj_faces[0]];
        for (size_t j=0; j<num_adj_faces; j++) {
            if (min_id > comp_ids[adj_faces[j]]) {
                min_id = comp_ids[adj_faces[j]];
            }
        }

        auto& m = index_map[i];
        m[min_id] = i;
        for (size_t j=0; j<num_adj_faces; j++) {
            const auto itr = m.find(comp_ids[adj_faces[j]]);
            if (itr == m.end()) {
                m[comp_ids[adj_faces[j]]] = num_vertices + added_vertices.size();
                added_vertices.push_back(i);
            }
        }
    }

    const size_t num_added_vertices = added_vertices.size();
    VectorF new_vertices((num_vertices + num_added_vertices) * dim);
    new_vertices.segment(0, num_vertices * dim) = vertices;
    for (size_t i=0; i<num_added_vertices; i++) {
        new_vertices.segment((num_vertices+i)*dim, dim) =
            vertices.segment(added_vertices[i]*dim, dim);
    }

    VectorI new_faces = faces;
    for (size_t i=0; i<num_faces; i++) {
        const auto id = comp_ids[i];
        for (size_t j=0; j<vertex_per_face; j++) {
            const auto& m = index_map[new_faces[i*vertex_per_face+j]];
            if (m.empty()) continue;
            assert(m.find(id) != m.end());
            new_faces[i*vertex_per_face+j] = m.at(id);
        }
    }

    return MeshFactory().load_data(new_vertices, new_faces, m_mesh->get_voxels(),
            dim, vertex_per_face, m_mesh->get_vertex_per_voxel()).create();
}

Mesh::Ptr MeshCutter::cut_at_uv_discontinuity() const {
    const size_t dim = m_mesh->get_dim();
    const size_t num_vertices = m_mesh->get_num_vertices();
    const size_t num_faces = m_mesh->get_num_faces();
    const size_t vertex_per_face = m_mesh->get_vertex_per_face();
    const auto& vertices = m_mesh->get_vertices();
    const auto& faces = m_mesh->get_faces();

    if (!m_mesh->has_attribute("corner_texture")) {
        throw RuntimeError("Mesh does not have texture");
    }
    const auto& uv = m_mesh->get_attribute("corner_texture");
    if (uv.size() != num_faces * vertex_per_face * 2) {
        throw RuntimeError("Comp ids size does not match the number of faces in mesh");
    }

    auto p_hash = [](const Vector2F& p) {
        return std::hash<Float>()(p[0] * 524287) ^ std::hash<Float>()(p[1]);
    };

    auto get_corner_index = [&faces, vertex_per_face](size_t fid, size_t vid) {
        for (size_t i=0; i<vertex_per_face; i++) {
            if (faces[fid*vertex_per_face+i] == vid) return i;
        }
        throw RuntimeError("Vertex " + std::to_string(vid)
                + " is not adjacent to face " + std::to_string(fid));
    };

    std::vector<size_t> added_vertices;
    using UVMap = std::unordered_map<Vector2F, size_t, decltype(p_hash)>;
    std::vector<UVMap> index_map(num_vertices, UVMap(8, p_hash));
    for (size_t i=0; i<num_vertices; i++) {
        const auto& adj_faces = m_mesh->get_vertex_adjacent_faces(i);
        const size_t num_adj_faces = adj_faces.size();
        if (num_adj_faces == 0) continue;

        MatrixFr local_uv(num_adj_faces, 2);
        for (size_t j=0; j<num_adj_faces; j++) {
            auto corner_id = get_corner_index(adj_faces[j], i);
            local_uv.row(j) = uv.segment<2>(
                    (adj_faces[j]*vertex_per_face + corner_id)*2);
        }

        bool is_interior_vtx = true;
        for (size_t j=1; is_interior_vtx && j<num_adj_faces; j++) {
            is_interior_vtx &= (local_uv.row(0) == local_uv.row(j));
        }
        if (is_interior_vtx) continue;

        Vector2F min_uv = local_uv.row(0);
        auto& m = index_map[i];
        m[min_uv] = i;
        for (size_t j=0; j<num_adj_faces; j++) {
            const auto itr = m.find(local_uv.row(j));
            if (itr == m.end()) {
                m[local_uv.row(j)] = num_vertices + added_vertices.size();
                added_vertices.push_back(i);
            }
        }
    }

    const size_t num_added_vertices = added_vertices.size();
    VectorF new_vertices((num_vertices + num_added_vertices) * dim);
    new_vertices.segment(0, num_vertices * dim) = vertices;
    for (size_t i=0; i<num_added_vertices; i++) {
        new_vertices.segment((num_vertices+i)*dim, dim) =
            vertices.segment(added_vertices[i]*dim, dim);
    }

    VectorI new_faces = faces;
    for (size_t i=0; i<num_faces; i++) {
        for (size_t j=0; j<vertex_per_face; j++) {
            const auto& m = index_map[new_faces[i*vertex_per_face+j]];
            if (m.empty()) continue;

            Vector2F uv_key = uv.segment<2>((i*vertex_per_face+j)*2);
            assert(m.find(uv_key) != m.end());
            new_faces[i*vertex_per_face+j] = m.at(uv_key);
        }
    }

    return MeshFactory().load_data(new_vertices, new_faces, m_mesh->get_voxels(),
            dim, vertex_per_face, m_mesh->get_vertex_per_voxel()).create();
}

