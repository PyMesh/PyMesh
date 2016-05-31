/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "EdgeThicknessParameterDerivative.h"

#include <cassert>
#include <vector>

using namespace PyMesh;

MatrixFr EdgeThicknessParameterDerivative::compute() {
    WireNetwork::Ptr wire_network = m_parameter->get_wire_network();

    const size_t dim = m_mesh->get_dim();
    const size_t num_mesh_vertices = m_mesh->get_num_vertices();
    const size_t num_mesh_faces = m_mesh->get_num_faces();
    const size_t num_wire_vertices = wire_network->get_num_vertices();
    const size_t num_wire_edges = wire_network->get_num_edges();
    const MatrixIr& wire_edges = wire_network->get_edges();

    assert(dim == wire_network->get_dim());

    std::vector<bool> in_roi(num_wire_edges, false);
    VectorI roi = m_parameter->get_roi();
    const size_t roi_size = roi.size();
    for (size_t i=0; i<roi_size; i++) {
        in_roi[roi[i]] = true;
    }

    MatrixFr derivative_v = MatrixFr::Zero(num_mesh_vertices, dim);
    VectorF weights = VectorF::Zero(num_mesh_vertices);

    for (size_t i=0; i<num_mesh_faces; i++) {
        const VectorI  face = m_mesh->get_face(i);
        const VectorF& normal = m_face_normals.row(i);
        int source = m_face_source[i];
        if (source < 0) {
            // Source is edge
            size_t edge_idx = -source - 1;
            assert(edge_idx >= 0 && edge_idx < num_wire_edges);
            if (in_roi[edge_idx]) {
                Float w0 = m_face_voronoi_areas(i, 0);
                Float w1 = m_face_voronoi_areas(i, 1);
                Float w2 = m_face_voronoi_areas(i, 2);

                derivative_v.row(face[0]) += w0 * normal.transpose();
                derivative_v.row(face[1]) += w1 * normal.transpose();
                derivative_v.row(face[2]) += w2 * normal.transpose();

                weights[face[0]] += w0;
                weights[face[1]] += w1;
                weights[face[2]] += w2;
            }
        }
    }

    for (size_t i=0; i<num_mesh_vertices; i++) {
        if (weights[i] > 0)
            derivative_v.row(i) /= weights[i];
    }
    return derivative_v;
}
