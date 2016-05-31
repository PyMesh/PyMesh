/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "VertexOffsetParameterDerivative.h"

using namespace PyMesh;

namespace VertexOffsetParameterDerivativeHelper {
    Float compute_projection_ratio(
            const VectorF& p, const VectorF& end_0, const VectorF& end_1) {
        VectorF v0 = end_1 - end_0;
        VectorF v1 = p - end_0;
        return v1.dot(v0) / v0.squaredNorm();
    }
}

using namespace VertexOffsetParameterDerivativeHelper;

MatrixFr VertexOffsetParameterDerivative::compute() {
    WireNetwork::Ptr wire_network = m_parameter->get_wire_network();

    const size_t dim = m_mesh->get_dim();
    const size_t num_mesh_vertices = m_mesh->get_num_vertices();
    const size_t num_mesh_faces = m_mesh->get_num_faces();
    const size_t num_wire_vertices = wire_network->get_num_vertices();

    assert(dim == wire_network->get_dim());

    VectorI roi = m_parameter->get_roi();
    BoolVector in_roi(num_wire_vertices, false);
    const size_t roi_size = roi.size();
    for (size_t i=0; i<roi_size; i++) {
        in_roi[roi[i]] = true;
    }

    VectorF weights = VectorF::Zero(num_mesh_vertices);
    MatrixFr derivative_v = MatrixFr::Zero(num_mesh_vertices, dim);
    MatrixFr wire_derivative = m_parameter->compute_derivative();

    for (size_t i=0; i< num_mesh_faces; i++) {
        int source = m_face_source[i];
        if (source < 0) {
            // Source is edge
            size_t edge_idx = -source - 1;
            compute_derivative_on_edge(
                    edge_idx, i, in_roi, wire_derivative,
                    derivative_v, weights);
        } else if (source > 0) {
            // Source is vertex
            size_t vertex_idx = source - 1;
            compute_derivative_on_vertex(
                    vertex_idx, i, in_roi, wire_derivative,
                    derivative_v, weights);
        }
    }

    for (size_t i=0; i<num_mesh_vertices; i++) {
        if (weights[i] > 0)
            derivative_v.row(i) /= weights[i];
    }
    return derivative_v;
}

void VertexOffsetParameterDerivative::compute_derivative_on_edge(
        size_t wire_edge_index,
        size_t face_index,
        const BoolVector& in_roi,
        const MatrixFr& wire_derivative,
        MatrixFr& derivative_v,
        VectorF& weights) {
    const Float eps = 1e-3;
    WireNetwork::Ptr wire_network = m_parameter->get_wire_network();

    const MatrixFr& wire_vertices = wire_network->get_vertices();
    const MatrixIr& wire_edges = wire_network->get_edges();

    const Vector2I& edge = wire_edges.row(wire_edge_index);
    const VectorF& v0 = wire_vertices.row(edge[0]);
    const VectorF& v1 = wire_vertices.row(edge[1]);

    const VectorI  face = m_mesh->get_face(face_index);
    const VectorF& face_normal = m_face_normals.row(face_index);
    const VectorF& c0 = m_mesh->get_vertex(face[0]);
    const VectorF& c1 = m_mesh->get_vertex(face[1]);
    const VectorF& c2 = m_mesh->get_vertex(face[2]);

    Float loc_0 = compute_projection_ratio(c0, v0, v1);
    Float loc_1 = compute_projection_ratio(c1, v0, v1);
    Float loc_2 = compute_projection_ratio(c2, v0, v1);

    assert(loc_0 >= -eps && loc_0 <= 1.0 + eps);
    assert(loc_1 >= -eps && loc_1 <= 1.0 + eps);
    assert(loc_2 >= -eps && loc_2 <= 1.0 + eps);

    Float w0 = m_face_voronoi_areas(face_index, 0);
    Float w1 = m_face_voronoi_areas(face_index, 1);
    Float w2 = m_face_voronoi_areas(face_index, 2);

    if (in_roi[edge[0]]) {
        derivative_v.row(face[0]) += w0 * (1.0 - loc_0) * wire_derivative.row(edge[0]);
        derivative_v.row(face[1]) += w1 * (1.0 - loc_1) * wire_derivative.row(edge[0]);
        derivative_v.row(face[2]) += w2 * (1.0 - loc_2) * wire_derivative.row(edge[0]);
    }

    if (in_roi[edge[1]]) {
        derivative_v.row(face[0]) += w0 * loc_0 * wire_derivative.row(edge[1]);
        derivative_v.row(face[1]) += w1 * loc_1 * wire_derivative.row(edge[1]);
        derivative_v.row(face[2]) += w2 * loc_2 * wire_derivative.row(edge[1]);
    }

    if (in_roi[edge[0]] || in_roi[edge[1]]) {
        weights[face[0]] += w0;
        weights[face[1]] += w1;
        weights[face[2]] += w2;
    }
}

void VertexOffsetParameterDerivative::compute_derivative_on_vertex(
        size_t wire_vertex_index,
        size_t face_index,
        const BoolVector& in_roi,
        const MatrixFr& wire_derivative,
        MatrixFr& derivative_v,
        VectorF& weights) {
    if (!in_roi[wire_vertex_index]) return;

    const VectorI face = m_mesh->get_face(face_index);

    Float w0 = m_face_voronoi_areas(face_index, 0);
    Float w1 = m_face_voronoi_areas(face_index, 1);
    Float w2 = m_face_voronoi_areas(face_index, 2);

    derivative_v.row(face[0]) = w0 * wire_derivative.row(wire_vertex_index);
    derivative_v.row(face[1]) = w1 * wire_derivative.row(wire_vertex_index);
    derivative_v.row(face[2]) = w2 * wire_derivative.row(wire_vertex_index);

    weights[face[0]] += w0;
    weights[face[1]] += w1;
    weights[face[2]] += w2;
}

