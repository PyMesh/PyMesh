#include "SimpleSubdivision.h"
#include <vector>
#include <Core/Exception.h>

void SimpleSubdivision::subdivide(MatrixFr vertices, MatrixIr faces,
        size_t num_iterations) {
    if (faces.cols() != 3) {
        throw NotImplementedError(
                "Only triangles mesh subdivision are supported!");
    }
    m_vertices = vertices;
    m_faces = faces;

    for (size_t i=0; i<num_iterations; i++) {
        subdivide_once();
    }
}

void SimpleSubdivision::subdivide_once() {
    m_mid_edge_points_map.clear();
    m_mid_edge_points.clear();
    m_subdivided_faces.clear();

    const size_t num_faces = m_faces.rows();
    for (size_t i=0; i<num_faces; i++) {
        subdivide_face(i);
    }

    update_vertices();
    update_faces();
}

void SimpleSubdivision::subdivide_face(size_t fi) {
    VectorI face = m_faces.row(fi);
    const size_t num_vertex_per_face = face.size();
    VectorI mid_edge_indices = compute_mid_edge_points(face);

    m_subdivided_faces.push_back(Vector3I(
                face[0],
                mid_edge_indices[0],
                mid_edge_indices[2]));
    m_subdivided_faces.push_back(Vector3I(
                face[1],
                mid_edge_indices[1],
                mid_edge_indices[0]));
    m_subdivided_faces.push_back(Vector3I(
                face[2],
                mid_edge_indices[2],
                mid_edge_indices[1]));
    m_subdivided_faces.push_back(Vector3I(
                mid_edge_indices[0],
                mid_edge_indices[1],
                mid_edge_indices[2]));
}

VectorI SimpleSubdivision::compute_mid_edge_points(const VectorI& face) {
    typedef std::map<Triplet, size_t>::const_iterator MapItr;
    const size_t num_vertex_per_face = face.size();
    VectorI mid_edge_indices(num_vertex_per_face);
    for (size_t j=0; j<num_vertex_per_face; j++) {
        size_t curr = j;
        size_t next = (j+1)%num_vertex_per_face;
        Triplet edge(face[curr], face[next]);

        VectorF v0 = m_vertices.row(face[curr]);
        VectorF v1 = m_vertices.row(face[next]);
        VectorF mid = 0.5 * (v0 + v1);

        MapItr itr = m_mid_edge_points_map.find(edge);
        if (itr == m_mid_edge_points_map.end()) {
            size_t idx = m_mid_edge_points.size();
            mid_edge_indices[j] = idx;
            m_mid_edge_points_map[edge] = idx;
            m_mid_edge_points.push_back(mid);
        } else {
            mid_edge_indices[j] = itr->second;
        }
    }
    return mid_edge_indices;
}

void SimpleSubdivision::update_vertices() {
    const size_t num_vertices = m_vertices.rows();
    const size_t dim = m_vertices.cols();
    const size_t num_mid_points = m_mid_edge_points.size();

    MatrixF vertices(num_vertices + num_mid_points, dim);

    vertices.block(0, 0, num_vertices, dim) = m_vertices;
    for (size_t i=0; i<num_mid_points; i++) {
        vertices.row(num_vertices+i) = m_mid_edge_points[i];
    }
    m_vertices = vertices;
}

void SimpleSubdivision::update_faces() {
    const size_t num_faces = m_subdivided_faces.size();
    const size_t num_vertex_per_face = m_faces.cols();
    m_faces.resize(num_faces, num_vertex_per_face);
    for (size_t i=0; i<num_faces; i++) {
        m_faces.row(i) = m_subdivided_faces[i];
    }
}
