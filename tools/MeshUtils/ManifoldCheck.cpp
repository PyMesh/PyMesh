/* This file is part of PyMesh. Copyright (c) 2018 by Qingnan Zhou */
#include "ManifoldCheck.h"
#include "EdgeUtils.h"
#include "MeshCutter.h"
#include <Math/MatrixUtils.h>

using namespace PyMesh;

VectorI ManifoldCheck::is_vertex_manifold(
        const MatrixIr& faces) {
    const size_t num_faces = faces.rows();
    if (num_faces == 0) return VectorI(0);
    const size_t num_vertices = faces.maxCoeff()+1;
    const size_t vertex_per_face = faces.cols();

    std::vector<std::vector<VectorI> > opposite_edges(num_vertices);
    if (vertex_per_face == 3) {
        for (size_t i=0; i<num_faces; i++) {
            const auto& f = faces.row(i);
            opposite_edges[f[0]].push_back(Vector2I(f[1], f[2]));
            opposite_edges[f[1]].push_back(Vector2I(f[2], f[0]));
            opposite_edges[f[2]].push_back(Vector2I(f[0], f[1]));
        }
    } else if (vertex_per_face == 4) {
        for (size_t i=0; i<num_faces; i++) {
            const auto& f = faces.row(i);
            opposite_edges[f[0]].push_back(Vector2I(f[1], f[2]));
            opposite_edges[f[0]].push_back(Vector2I(f[2], f[3]));
            opposite_edges[f[1]].push_back(Vector2I(f[2], f[3]));
            opposite_edges[f[1]].push_back(Vector2I(f[3], f[0]));
            opposite_edges[f[2]].push_back(Vector2I(f[3], f[0]));
            opposite_edges[f[2]].push_back(Vector2I(f[0], f[1]));
            opposite_edges[f[3]].push_back(Vector2I(f[0], f[1]));
            opposite_edges[f[3]].push_back(Vector2I(f[1], f[2]));
        }
    } else {
        std::stringstream err_msg;
        err_msg << "Vertex manifold check does not support face with "
            << vertex_per_face << " vertices.";
        throw NotImplementedError(err_msg.str());
    }

    VectorI is_manifold(num_vertices);
    is_manifold.setConstant(1.0);

    for (size_t i=0; i<num_vertices; i++) {
        const auto& entries = opposite_edges[i];
        if (entries.empty()) continue;
        try {
            auto edge_loops = EdgeUtils::chain_edges(
                    MatrixUtils::rowstack(entries));
            is_manifold[i] = edge_loops.size() == 1;
        } catch (...) {
            is_manifold[i] = 0;
        }
    }
    return is_manifold;
}

MatrixIr ManifoldCheck::is_edge_manifold(const MatrixIr& faces) {
    const size_t num_faces = faces.rows();
    const size_t vertex_per_face = faces.cols();
    auto edge_map = EdgeUtils::compute_edge_face_adjacency(faces);
    MatrixIr is_manifold(num_faces, vertex_per_face);
    for (size_t i=0; i<num_faces; i++) {
        for (size_t j=0; j<vertex_per_face; j++) {
            const auto itr = edge_map.find(
                    {faces(i,j), faces(i,(j+1)%vertex_per_face)});
            assert(itr != edge_map.end());
            is_manifold(i,j) = itr->second.size() > 2 ? 0 : 1;
        }
    }
    return is_manifold;
}


Mesh::Ptr ManifoldCheck::cut_to_manifold(Mesh::Ptr mesh) {
    const size_t num_faces = mesh->get_num_faces();
    const size_t vertex_per_face = mesh->get_vertex_per_face();
    if (vertex_per_face != 3) {
        throw RuntimeError("Only triangle mesh is supported by cut_to_manifold.");
    }
    const MatrixIr& faces = MatrixUtils::reshape<MatrixIr>(
            mesh->get_faces(), mesh->get_num_faces(), 3);
    MatrixIr edge_manifold = is_edge_manifold(faces);

    std::vector<std::vector<int>> non_manifold_edges;
    for (size_t i=0; i<num_faces; i++) {
        if (edge_manifold(i, 0) < 0.5) {
            non_manifold_edges.push_back({faces(i,0), faces(i,1)});
        }
        if (edge_manifold(i, 1) < 0.5) {
            non_manifold_edges.push_back({faces(i,1), faces(i,2)});
        }
        if (edge_manifold(i, 2) < 0.5) {
            non_manifold_edges.push_back({faces(i,2), faces(i,0)});
        }
    }

    MeshCutter cutter(mesh);
    return cutter.cut_along_edges(non_manifold_edges);
}

