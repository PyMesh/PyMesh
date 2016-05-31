/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "SymmetryOperators.h"

#include <Misc/HashGrid.h>
#include <Misc/TripletMap.h>

using namespace PyMesh;

namespace SymmetryOperatorsHelper {
    HashGrid::Ptr compute_vertex_lookup_grid(const MatrixFr& vertices,
            Float cell_size) {
        const size_t num_vertices = vertices.rows();
        const size_t dim = vertices.cols();
        HashGrid::Ptr grid = HashGrid::create(cell_size, dim);

        VectorI v_indices(num_vertices);
        for (size_t i=0; i<num_vertices; i++)
            v_indices[i] = i;

        grid->insert_multiple(v_indices, vertices);
        return grid;
    }

    TripletMap<size_t> create_edge_lookup_map(const MatrixIr& edges) {
        TripletMap<size_t> edge_map;
        const size_t num_edges = edges.rows();
        for (size_t i=0; i<num_edges; i++) {
            Triplet key(edges(i, 0), edges(i, 1));
            edge_map.insert(key, i);
        }
        return edge_map;
    }
}
using namespace SymmetryOperatorsHelper;

template<>
SymmetryOperators::Operators SymmetryOperators::generate_reflective_symmetry_operators<2>(const VectorF& c) {
    assert(c.size() == 2);
    Operators ops;

    // Reflective symmetry with respect to X and Y axis assuming origin at c.
    ops.push_back([=](const VectorF& p) { return Vector2F(2*c[0]-p[0],        p[1]); });
    ops.push_back([=](const VectorF& p) { return Vector2F(       p[0], 2*c[1]-p[1]); });

    // Reflective symmetry with respect to center.
    ops.push_back([=](const VectorF& p) { return Vector2F(2*c[0]-p[0], 2*c[1]-p[1]); });

    return ops;
}

template<>
SymmetryOperators::Operators SymmetryOperators::generate_reflective_symmetry_operators<3>(const VectorF& c) {
    assert(c.size() == 3);
    Operators ops;

    // Reflective symmetry with respect to YZ, ZX and XY plane.
    ops.push_back([=](const VectorF& p) { return Vector3F(2*c[0]-p[0],        p[1],        p[2]); });
    ops.push_back([=](const VectorF& p) { return Vector3F(       p[0], 2*c[1]-p[1],        p[2]); });
    ops.push_back([=](const VectorF& p) { return Vector3F(       p[0],        p[1], 2*c[2]-p[2]); });

    // Reflective symmetry with respect X, Y and Z axis.
    ops.push_back([=](const VectorF& p) { return Vector3F(2*c[0]-p[0], 2*c[1]-p[1],        p[2]); });
    ops.push_back([=](const VectorF& p) { return Vector3F(       p[0], 2*c[1]-p[1], 2*c[2]-p[2]); });
    ops.push_back([=](const VectorF& p) { return Vector3F(2*c[0]-p[0],        p[1], 2*c[2]-p[2]); });

    // Reflective symmetry with respect to center.
    ops.push_back([=](const VectorF& p) { return Vector3F(2*c[0]-p[0], 2*c[1]-p[1], 2*c[2]-p[2]); });

    return ops;
}

template<>
SymmetryOperators::Operators SymmetryOperators::generate_rotational_symmetry_operators<2>(const VectorF& c) {
    assert(c.size() == 2);
    Operators ops;

    ops.push_back([=](const VectorF& p) {
            return Vector2F(c[0]+p[1]-c[1], c[1]-p[0]+c[0]); });
    ops.push_back([=](const VectorF& p) {
            return Vector2F(c[0]-p[1]+c[1], c[1]+p[0]-c[0]); });
    ops.push_back([=](const VectorF& p) {
            return Vector2F(c[0]-p[0]+c[0], c[1]-p[1]+c[1]); });

    return ops;
}

template<>
SymmetryOperators::Operators SymmetryOperators::generate_rotational_symmetry_operators<3>(const VectorF& c) {
    assert(c.size() == 3);
    Operators ops;

    ops.push_back([=](const VectorF& p)
            { return Vector3F(p[0], c[1]+p[2]-c[2], c[2]-p[1]+c[1]); });
    ops.push_back([=](const VectorF& p)
            { return Vector3F(p[0], c[1]-p[1]+c[1], c[2]-p[2]+c[2]); });
    ops.push_back([=](const VectorF& p)
            { return Vector3F(p[0], c[1]-p[2]+c[2], c[2]+p[1]-c[1]); });

    ops.push_back([=](const VectorF& p)
            { return Vector3F(c[2]+p[0]-c[0], p[1], c[0]-p[2]+c[2]); });
    ops.push_back([=](const VectorF& p)
            { return Vector3F(c[0]-p[0]+c[0], p[1], c[2]-p[2]+c[2]); });
    ops.push_back([=](const VectorF& p)
            { return Vector3F(c[2]-p[0]+c[0], p[1], c[0]+p[2]-c[2]); });

    ops.push_back([=](const VectorF& p)
            { return Vector3F(c[0]+p[1]-c[1], c[1]-p[0]+c[0], p[2]); });
    ops.push_back([=](const VectorF& p)
            { return Vector3F(c[0]-p[0]+c[0], c[1]-p[1]+c[1], p[2]); });
    ops.push_back([=](const VectorF& p)
            { return Vector3F(c[0]-p[1]+c[1], c[1]+p[0]-c[0], p[2]); });

    return ops;
}

SymmetryOperators::SymmetryConnectivity SymmetryOperators::compute_vertex_connectivity(
        const MatrixFr& vertices, SymmetryOperators::Operators& ops) {
    const Float tol = 1e-12;
    const size_t num_vertices = vertices.rows();
    const Float cell_size = 1e-6;
    HashGrid::Ptr grid = compute_vertex_lookup_grid(vertices, cell_size);

    SymmetryConnectivity connectivity(num_vertices);
    for (auto& op : ops) {
        for (size_t i=0; i<num_vertices; i++) {
            const VectorF& curr_p  = vertices.row(i);
            VectorF mapped_p = op(curr_p);
            VectorI candidates = grid->get_items_near_point(mapped_p);
            const size_t num_candidates = candidates.size();
            for (size_t j=0; j<num_candidates; j++) {
                const VectorF& other_p = vertices.row(candidates[j]);
                Float squared_dist = (mapped_p - other_p).squaredNorm();
                if (squared_dist < tol) {
                    connectivity[i].insert(candidates[j]);
                    connectivity[candidates[j]].insert(i);
                }
            }
        }
    }
    return connectivity;
}

//SymmetryOperators::SymmetryConnectivity SymmetryOperators::compute_vertex_connectivity(
//        const MatrixFr& vertices, SymmetryOperators::Operators& ops) {
//    const Float tol = 1e-12;
//    const size_t num_vertices = vertices.rows();
//
//    SymmetryConnectivity connectivity(num_vertices);
//    for (auto& op : ops) {
//        std::vector<bool> visited(num_vertices, false);
//        for (size_t i=0; i<num_vertices; i++) {
//            if (visited[i]) continue;
//
//            const VectorF& curr_p = vertices.row(i);
//            VectorF mapped_p = op(curr_p);
//            for (size_t j=i+1; j<num_vertices; j++) {
//                const VectorF& other_p = vertices.row(j);
//                Float squared_dist = (mapped_p - other_p).squaredNorm();
//                if (squared_dist < tol) {
//                    visited[i] = true;
//                    visited[j] = true;
//                    connectivity[i].push_back(j);
//                    connectivity[j].push_back(i);
//                }
//            }
//        }
//    }
//
//    return connectivity;
//}

SymmetryOperators::SymmetryConnectivity SymmetryOperators::compute_edge_connectivity(
        const MatrixFr& vertices,
        const MatrixIr& edges,
        SymmetryOperators::Operators& ops) {
    const Float tol = 1e-12;
    const size_t num_edges = edges.rows();
    const Float cell_size = 1e-6;
    HashGrid::Ptr grid = compute_vertex_lookup_grid(vertices, cell_size);
    auto edge_map = create_edge_lookup_map(edges);

    SymmetryConnectivity connectivity(num_edges);
    for (auto& op : ops) {
        for (size_t i=0; i<num_edges; i++) {
            const VectorI& curr_e = edges.row(i);
            const VectorF& e0 = vertices.row(curr_e[0]);
            const VectorF& e1 = vertices.row(curr_e[1]);
            VectorF mapped_e0 = op(e0);
            VectorF mapped_e1 = op(e1);

            VectorI candidate_e0 = grid->get_items_near_point(mapped_e0);
            VectorI candidate_e1 = grid->get_items_near_point(mapped_e1);
            const size_t num_candidates_e0 = candidate_e0.size();
            const size_t num_candidates_e1 = candidate_e1.size();

            for (size_t j=0; j<num_candidates_e0; j++) {
                const size_t e0_idx = candidate_e0[j];
                const VectorF& other_e0 = vertices.row(e0_idx);
                if ((mapped_e0 - other_e0).squaredNorm() > tol) continue;

                for (size_t k=0; k<num_candidates_e1; k++) {
                    const size_t e1_idx = candidate_e1[k];
                    const VectorF& other_e1 = vertices.row(e1_idx);
                    if ((mapped_e1 - other_e1).squaredNorm() > tol) continue;

                    Triplet key(e0_idx, e1_idx);
                    auto itr = edge_map.find(key);
                    if (itr != edge_map.end()) {
                        assert(itr->second.size() == 1);
                        const size_t mapped_edge_index = itr->second.front();
                        connectivity[i].insert(mapped_edge_index);
                        connectivity[mapped_edge_index].insert(i);
                    }
                }
            }
        }
    }
    return connectivity;
}

//SymmetryOperators::SymmetryConnectivity SymmetryOperators::compute_edge_connectivity(
//        const MatrixFr& vertices,
//        const MatrixIr& edges,
//        SymmetryOperators::Operators& ops) {
//    const Float tol = 1e-12;
//    const size_t num_edges = edges.rows();
//
//    SymmetryConnectivity connectivity(num_edges);
//    for (auto& op : ops) {
//        std::vector<bool> visited(num_edges, false);
//        for (size_t i=0; i<num_edges; i++) {
//            if (visited[i]) continue;
//
//            const VectorI& curr_e = edges.row(i);
//            const VectorF& e0 = vertices.row(curr_e[0]);
//            const VectorF& e1 = vertices.row(curr_e[1]);
//            VectorF mapped_e0 = op(e0);
//            VectorF mapped_e1 = op(e1);
//
//            for (size_t j=i+1; j<num_edges; j++) {
//                const VectorI& other_e = edges.row(j);
//                const VectorF& other_e0 = vertices.row(other_e[0]);
//                const VectorF& other_e1 = vertices.row(other_e[1]);
//
//                Float dist_1 = (mapped_e0 - other_e0).squaredNorm() +
//                    (mapped_e1 - other_e1).squaredNorm();
//                Float dist_2 = (mapped_e0 - other_e1).squaredNorm() +
//                    (mapped_e1 - other_e0).squaredNorm();
//                if (dist_1 < tol || dist_2 < tol) {
//                    visited[i] = true;
//                    visited[j] = true;
//                    connectivity[i].push_back(j);
//                    connectivity[j].push_back(i);
//                }
//            }
//        }
//    }
//
//    return connectivity;
//}

VectorI SymmetryOperators::label_connected_components(size_t num_entries,
        const SymmetryOperators::SymmetryConnectivity& connectivity) {
    size_t count = 0;
    VectorI components = VectorI::Ones(num_entries) * -1;
    for (size_t i=0; i<num_entries; i++) {
        if (components[i] >= 0) continue;

        std::queue<size_t> Q;
        Q.push(i);
        components[i] = count;

        while (!Q.empty()) {
            size_t curr = Q.front();
            Q.pop();
            for (auto neighbor : connectivity[curr]) {
                if (components[neighbor] == -1) {
                    Q.push(neighbor);
                    components[neighbor] = count;
                }
            }
        }
        count++;
    }
    return components;
}

