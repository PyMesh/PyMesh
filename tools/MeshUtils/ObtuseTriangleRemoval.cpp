#include "ObtuseTriangleRemoval.h"

#include <cmath>
#include <iostream>

#include "IndexHeap.h"

namespace ObtuseTriangleRemovalHelper {
    Float angle(const Vector3F& v1, const Vector3F& v2) {
        Float sin_val = v1.cross(v2).norm();
        Float cos_val = v1.dot(v2);
        Float result = atan2(sin_val, cos_val);
        return result;
    }
}

using namespace ObtuseTriangleRemovalHelper;

ObtuseTriangleRemoval::ObtuseTriangleRemoval(MatrixF& vertices, MatrixI& faces)
    : m_vertices(vertices), m_faces(faces) { }

size_t ObtuseTriangleRemoval::run(Float max_angle_allowed) {
    set_all_faces_as_valid();
    compute_face_angles();
    compute_opposite_vertices();
    compute_edge_face_adjacency();

    return split_obtuse_triangles(max_angle_allowed);
}

MatrixF ObtuseTriangleRemoval::get_vertices() {
    const size_t num_old_v = m_vertices.rows();
    const size_t num_new_v = m_new_vertices.size();
    const size_t num_vertices = num_old_v + num_new_v;
    MatrixF vertices(num_vertices, 3);
    vertices.topRows(num_old_v) = m_vertices;
    size_t count = num_old_v;
    for (std::vector<Vector3F>::const_iterator itr = m_new_vertices.begin();
            itr != m_new_vertices.end(); itr++) {
        vertices.row(count) = *itr;
        count ++;
    }
    return vertices;
}

MatrixI ObtuseTriangleRemoval::get_faces() {
    assert(m_valid.size() == m_faces.rows());
    typedef std::vector<Vector3I> FaceArray;
    const size_t num_ori_f = m_faces.rows();
    FaceArray valid_faces;
    valid_faces.reserve(num_ori_f);
    for (size_t i=0; i<num_ori_f; i++) {
        if (m_valid[i]) {
            valid_faces.push_back(m_faces.row(i));
        }
    }

    const size_t num_old_f = valid_faces.size();
    const size_t num_new_f = m_new_faces.size();
    const size_t num_faces = num_old_f + num_new_f;
    MatrixI faces(num_faces, 3);

    size_t count = 0;
    for (FaceArray::const_iterator itr = valid_faces.begin();
            itr != valid_faces.end(); itr++) {
        faces.row(count) = *itr;
        count ++;
    }

    for (FaceArray::const_iterator itr = m_new_faces.begin();
            itr != m_new_faces.end(); itr++) {
        faces.row(count) = *itr;
        count ++;
    }
    return faces;
}

void ObtuseTriangleRemoval::set_all_faces_as_valid() {
    m_valid = std::vector<bool>(m_faces.rows(), true);
}

void ObtuseTriangleRemoval::compute_face_angles() {
    const size_t num_faces = m_faces.rows();
    m_face_angles.clear();
    m_face_angles.reserve(num_faces * 3);
    for (size_t i=0; i<num_faces; i++) {
        Vector3F v1 = m_vertices.row(m_faces(i, 0));
        Vector3F v2 = m_vertices.row(m_faces(i, 1));
        Vector3F v3 = m_vertices.row(m_faces(i, 2));

        m_face_angles.push_back(angle(v2-v1, v3-v1));
        m_face_angles.push_back(angle(v3-v2, v1-v2));
        m_face_angles.push_back(angle(v1-v3, v2-v3));
    }
}

void ObtuseTriangleRemoval::compute_opposite_vertices() {
    const size_t num_faces = m_faces.rows();
    m_opp_vertices.clear();
    m_opp_vertices.reserve(num_faces * 3);
    for (size_t i=0; i<num_faces; i++) {
        m_opp_vertices.push_back(0);
        m_opp_vertices.push_back(1);
        m_opp_vertices.push_back(2);
    }
}

void ObtuseTriangleRemoval::compute_edge_face_adjacency() {
    const size_t num_faces = m_faces.rows();
    m_edges.clear();
    m_edges.reserve(num_faces * 3);
    for (size_t i=0; i<num_faces; i++) {
        Edge e1(m_faces(i, 1), m_faces(i, 2));
        Edge e2(m_faces(i, 0), m_faces(i, 2));
        Edge e3(m_faces(i, 0), m_faces(i, 1));

        m_edges.push_back(e1);
        m_edges.push_back(e2);
        m_edges.push_back(e3);
    }

    for (size_t i=0; i<3*num_faces; i++) {
        Edge& edge = m_edges[i];
        m_edge_faces.insert(edge, i);
    }
}

bool ObtuseTriangleRemoval::edge_can_be_splited(size_t ext_idx) const {
    bool result = true;
    const Edge& e = m_edges[ext_idx];
    const AdjFaces& adj_faces = m_edge_faces.get(e);
    for (AdjFaces::const_iterator itr = adj_faces.begin();
            itr != adj_faces.end(); itr++) {
        size_t f_ext_idx = *itr;
        size_t f_idx = f_ext_idx / 3;
        result &= m_valid[f_idx];
    }
    return result;
}

size_t ObtuseTriangleRemoval::split_obtuse_triangles(Float max_angle) {
    size_t num_splited = 0;
    IndexHeap<Float> candidates(m_face_angles);
    while (!candidates.empty()) {
        size_t ext_idx = candidates.top();
        candidates.pop();
        if (m_face_angles[ext_idx] < max_angle) break;
        if (!edge_can_be_splited(ext_idx)) continue;

        split_triangle(ext_idx);
        num_splited ++;
    }
    return num_splited;
}

void ObtuseTriangleRemoval::split_triangle(size_t ext_idx) {
    Edge edge = m_edges[ext_idx];

    Vector3F proj_point = project(ext_idx);
    m_new_vertices.push_back(proj_point);
    const size_t num_old_v = m_vertices.rows();
    size_t v_mid = num_old_v + m_new_vertices.size() - 1;

    const AdjFaces& adj_faces = m_edge_faces[edge];
    for (AdjFaces::const_iterator itr = adj_faces.begin();
            itr != adj_faces.end(); itr++) {
        size_t f_ext_idx = *itr;
        size_t f_idx = f_ext_idx / 3;
        size_t opp_idx = m_opp_vertices[f_ext_idx];
        size_t v0 = m_faces(f_idx, opp_idx);
        size_t v1 = m_faces(f_idx, (opp_idx+1)%3);
        size_t v2 = m_faces(f_idx, (opp_idx+2)%3);

        Vector3I f1(v0, v1, v_mid);
        Vector3I f2(v0, v_mid, v2);

        m_new_faces.push_back(f1);
        m_new_faces.push_back(f2);

        m_valid[f_idx] = false;
    }
}

Vector3F ObtuseTriangleRemoval::project(size_t ext_idx) {
    size_t f_idx = ext_idx / 3;
    size_t opp_idx = m_opp_vertices[ext_idx];
    Edge& e = m_edges[ext_idx];
    Vector3F v1 = m_vertices.row(e.get_ori_data()[0]);
    Vector3F v2 = m_vertices.row(e.get_ori_data()[1]);
    Vector3F v3 = m_vertices.row(m_faces(f_idx, opp_idx));

    Vector3F e1 = v2 - v1;
    Vector3F e2 = v3 - v1;
    Vector3F offset = e1.dot(e2) / e1.squaredNorm() * e1;
    Vector3F proj = v1 + offset;
    return proj;
}

