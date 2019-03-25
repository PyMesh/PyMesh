/* This file is part of PyMesh. Copyright (c) 2018 by Qingnan Zhou */
#include "MeshCutter.h"
#include "EdgeUtils.h"

#include <cassert>
#include <array>
#include <iostream>
#include <list>
#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>

#include <MeshFactory.h>
#include <Core/Exception.h>
#include <Misc/MultipletMap.h>

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

    struct Vector2FHash {
        size_t operator()(const Vector2F& p) const {
            return std::hash<Float>()(p[0] * 524287) ^ std::hash<Float>()(p[1]);
        }
    };

    auto get_corner_index = [&faces, vertex_per_face](size_t fid, size_t vid) {
        for (size_t i=0; i<vertex_per_face; i++) {
            if (faces[fid*vertex_per_face+i] == vid) return i;
        }
        throw RuntimeError("Vertex " + std::to_string(vid)
                + " is not adjacent to face " + std::to_string(fid));
    };

    std::vector<size_t> added_vertices;
    using UVMap = std::unordered_map<Vector2F, size_t, Vector2FHash>;
    std::vector<UVMap> index_map(num_vertices, UVMap(8));
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

Mesh::Ptr MeshCutter::cut_along_edges(
        const std::vector<std::vector<int>>& edge_chains) const {
    std::unordered_set<Duplet, MultipletHashFunc<Duplet>> cut_edges;
    for (const auto& chain : edge_chains) {
        const auto chain_size = chain.size();
        assert(chain_size >= 2);
        for (size_t i=0; i < chain_size-1; i++) {
            cut_edges.insert(Duplet(
                    static_cast<int>(chain[i]),
                    static_cast<int>(chain[i+1]) ));
        }
    }
    auto is_cut_edge = [&cut_edges](const Duplet& e) {
        return cut_edges.find(e) != cut_edges.end();
    };

    const size_t dim = m_mesh->get_dim();
    const size_t vertex_per_face = m_mesh->get_vertex_per_face();
    const size_t num_existing_vertices = m_mesh->get_num_vertices();
    auto faces = m_mesh->get_faces(); // Intentional copy
    const auto& vertices = m_mesh->get_vertices();
    std::vector<size_t> new_vertices;

    const size_t num_vertices = m_mesh->get_num_vertices();
    for (size_t i=0; i<num_vertices; i++) {
        const auto& adj_faces = m_mesh->get_vertex_adjacent_faces(i);
        const auto num_adj_faces = adj_faces.size();
        if (num_adj_faces == 0) continue;
        DupletMap<int> edge_face_map;
        std::unordered_map<int, std::array<Duplet, 2>> spokes;
        for (size_t j=0; j<num_adj_faces; j++) {
            std::array<Duplet, 2> es;
            const auto fid = adj_faces[j];
            const auto& f = faces.segment<3>(fid*3);
            if (f[0] == i) {
                es[0] = {f[0], f[1]};
                es[1] = {f[0], f[2]};
            } else if (f[1] == i) {
                es[0] = {f[1], f[2]};
                es[1] = {f[1], f[0]};
            } else {
                assert(f[2] == i);
                es[0] = {f[2], f[0]};
                es[1] = {f[2], f[1]};
            }
            spokes.insert({fid, es});
            edge_face_map.insert(es[0], fid);
            edge_face_map.insert(es[1], fid);
        }

        std::unordered_set<int> visited;
        auto has_visited = [&visited](int fid) {
            return visited.find(fid) != visited.end();
        };
        std::vector<std::list<int>> comps;

        auto flood_across_edge = [
            &is_cut_edge,
            &has_visited,
            &edge_face_map,
            &visited
        ](const Duplet& e, auto& Q) {
            if (!is_cut_edge(e)) {
                const auto& adj_f_to_e = edge_face_map[e];
                for (auto adj_f : adj_f_to_e) {
                    if (!has_visited(adj_f)) {
                        Q.push(adj_f);
                        visited.insert(adj_f);
                    }
                }
            }
        };

        for (size_t j=0; j<num_adj_faces; j++) {
            if (has_visited(adj_faces[j])) continue;

            std::list<int> comp;
            std::queue<int> Q;
            Q.push(adj_faces[j]);
            visited.insert(adj_faces[j]);
            while(!Q.empty()) {
                const auto fid = Q.front();
                Q.pop();
                comp.push_back(fid);

                const auto& e0 = spokes[fid][0];
                flood_across_edge(e0, Q);
                const auto& e1 = spokes[fid][1];
                flood_across_edge(e1, Q);
            }
            comps.push_back(comp);
        }

        const size_t num_comps = comps.size();
        if (num_comps > 1) {
            for (size_t j=1; j<num_comps; j++) {
                const int new_vid = num_existing_vertices + new_vertices.size();
                for (const auto fid : comps[j]) {
                    if (faces[fid*vertex_per_face] == i)
                        faces[fid*vertex_per_face] = new_vid;
                    else if (faces[fid*vertex_per_face+1] == i)
                        faces[fid*vertex_per_face+1] = new_vid;
                    else if (faces[fid*vertex_per_face+2] == i)
                        faces[fid*vertex_per_face+2] = new_vid;
                    else {
                        throw RuntimeError("faces does not contain the given vertex");
                    }
                }
                new_vertices.push_back(i);
            }
        }
    }

    const size_t num_new_vertices = new_vertices.size();
    VectorF out_vertices((num_existing_vertices + num_new_vertices)*dim);
    out_vertices.segment(0, num_existing_vertices * dim) = vertices;
    for (size_t i=0; i<num_new_vertices; i++) {
        out_vertices.segment((num_existing_vertices+i)*dim, dim) =
            vertices.segment(new_vertices[i]*dim, dim);
    }

    return MeshFactory().load_data(out_vertices, faces, m_mesh->get_voxels(),
            dim, vertex_per_face, m_mesh->get_vertex_per_voxel()).create();
}

