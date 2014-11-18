#include "VertexOffsetParameterDerivative.h"

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

    VectorI num_adj_faces_in_roi = VectorI::Zero(num_mesh_vertices);
    MatrixFr derivative_v = MatrixFr::Zero(num_mesh_vertices, dim);
    MatrixFr wire_derivative = m_parameter->compute_derivative();

    for (size_t i=0; i< num_mesh_faces; i++) {
        int source = m_face_source[i];
        if (source < 0) {
            // Source is edge
            size_t edge_idx = -source - 1;
            compute_derivative_on_edge(
                    edge_idx, i, in_roi, wire_derivative,
                    derivative_v, num_adj_faces_in_roi);
        } else if (source > 0) {
            // Source is vertex
            size_t vertex_idx = source - 1;
            compute_derivative_on_vertex(
                    vertex_idx, i, in_roi, wire_derivative,
                    derivative_v, num_adj_faces_in_roi);
        }
    }

    for (size_t i=0; i<num_mesh_vertices; i++) {
        if (num_adj_faces_in_roi[i] > 0)
            derivative_v.row(i) /= num_adj_faces_in_roi[i];
    }
    return derivative_v;
}

void VertexOffsetParameterDerivative::compute_derivative_on_edge(
        size_t wire_edge_index,
        size_t face_index,
        const BoolVector& in_roi,
        const MatrixFr& wire_derivative,
        MatrixFr& derivative_v,
        VectorI& num_adj_faces_in_roi) {
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

    assert(loc_0 >= 0.0 && loc_0 <= 1.0);
    assert(loc_1 >= 0.0 && loc_1 <= 1.0);
    assert(loc_2 >= 0.0 && loc_2 <= 1.0);

    if (in_roi[edge[0]]) {
        derivative_v.row(face[0]) += (1.0 - loc_0) * wire_derivative.row(edge[0]);
        derivative_v.row(face[1]) += (1.0 - loc_1) * wire_derivative.row(edge[0]);
        derivative_v.row(face[2]) += (1.0 - loc_2) * wire_derivative.row(edge[0]);
    }

    if (in_roi[edge[1]]) {
        derivative_v.row(face[0]) += loc_0 * wire_derivative.row(edge[1]);
        derivative_v.row(face[1]) += loc_1 * wire_derivative.row(edge[1]);
        derivative_v.row(face[2]) += loc_2 * wire_derivative.row(edge[1]);
    }

    if (in_roi[edge[0]] || in_roi[edge[1]]) {
        num_adj_faces_in_roi[face[0]]++;
        num_adj_faces_in_roi[face[1]]++;
        num_adj_faces_in_roi[face[2]]++;
    }
}

void VertexOffsetParameterDerivative::compute_derivative_on_vertex(
        size_t wire_vertex_index,
        size_t face_index,
        const BoolVector& in_roi,
        const MatrixFr& wire_derivative,
        MatrixFr& derivative_v,
        VectorI& num_adj_faces_in_roi) {
    if (!in_roi[wire_vertex_index]) return;

    const VectorI face = m_mesh->get_face(face_index);
    derivative_v.row(face[0]) = wire_derivative.row(wire_vertex_index);
    derivative_v.row(face[1]) = wire_derivative.row(wire_vertex_index);
    derivative_v.row(face[2]) = wire_derivative.row(wire_vertex_index);

    num_adj_faces_in_roi[face[0]]++;
    num_adj_faces_in_roi[face[1]]++;
    num_adj_faces_in_roi[face[2]]++;
}

