/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "EdgeSplitter.h"

#include <iostream>
#include <list>
#include <sstream>

#include <Core/Exception.h>
#include <Math/MatrixUtils.h>
#include <Misc/Triplet.h>
#include <MeshUtils/IndexHeap.h>

using namespace PyMesh;

namespace EdgeSplitterHelper {
    void split(std::list<VectorF>& end_points,
            std::list<VectorF>::iterator begin_itr,
            Float length, Float threshold) {
        if (length < threshold) return;

        auto end_itr = std::next(begin_itr);
        assert(end_itr != end_points.end());
        const auto& v1 = *begin_itr;
        const auto& v2 = *end_itr;
        VectorF mid = 0.5 * (v1 + v2);
        Float half_length = length * 0.5;
        auto mid_itr = end_points.insert(end_itr, mid);

        split(end_points, begin_itr, half_length, threshold);
        split(end_points, mid_itr, half_length, threshold);
    }

    size_t get_opposite_vertex_index(const Eigen::Ref<VectorI>& f,
            size_t vi, size_t vj) {
        if (f[0] != vi && f[0] != vj) return f[0];
        if (f[1] != vi && f[1] != vj) return f[1];
        if (f[2] != vi && f[2] != vj) return f[2];
        std::stringstream err_msg;
        err_msg << "Invalid face: <"
            << f[0] << ", "
            << f[1] << ", "
            << f[2] << "> encountered when looking for opposite vertex of edge <"
            << vi << ", " << vj << ">";
        throw RuntimeError(err_msg.str());
    }

    bool has_same_orientation(const Eigen::Ref<VectorI>& f,
            size_t v0, size_t v1, size_t v2) {
        if (f[0] == v0) {
            if (f[1] == v1 && f[2] == v2) return true;
            else return false;
        } else if (f[0] == v1) {
            if (f[1] == v2 && f[2] == v0) return true;
            else return false;
        } else {
            assert(f[0] == v2);
            if (f[1] == v0 && f[2] == v1) return true;
            else return false;
        }
    }
}
using namespace EdgeSplitterHelper;

EdgeSplitter::EdgeSplitter(const MatrixFr& vertices, const MatrixIr& faces) :
    m_vertices(vertices), m_faces(faces) {
        if (m_faces.cols() != 3) {
            throw NotImplementedError(
                    "Edge splitter only support triangular mesh");
        }
}

void EdgeSplitter::run(IndicatorFunc split_indicator, Float max_length) {
    clear_intermediate_data();
    generate_edge_map();
    set_all_faces_as_valid();
    for (const auto itr : m_edge_adj_faces) {
        const auto& edge = itr.first;
        const auto& adj_faces = itr.second;
        const auto& e = edge.get_ori_data().segment<2>(0);
        const auto v0 = m_vertices.row(e[0]);
        const auto v1 = m_vertices.row(e[1]);
        if (split_indicator(v0, v1)) {
            split_edge(e, adj_faces, max_length);
        }
    }
    finalize_geometry();
}

void EdgeSplitter::clear_intermediate_data() {
    m_edge_adj_faces.clear();
    m_face_is_valid.clear();
    m_new_vertices.clear();
    m_new_faces.clear();
}

void EdgeSplitter::generate_edge_map() {
    const size_t num_faces = m_faces.rows();
    for (size_t i=0; i<num_faces; i++) {
        const auto& f = m_faces.row(i);

        m_edge_adj_faces.insert(Triplet(f[1], f[2]), i);
        m_edge_adj_faces.insert(Triplet(f[2], f[0]), i);
        m_edge_adj_faces.insert(Triplet(f[0], f[1]), i);
    }
}

void EdgeSplitter::set_all_faces_as_valid() {
    const size_t num_faces = m_faces.rows();
    m_face_is_valid = std::vector<bool>(num_faces, true);
}

void EdgeSplitter::split_edge(const VectorI& edge,
        const std::vector<size_t>& adj_faces, Float max_length) {
    const auto v0 = m_vertices.row(edge[0]);
    const auto v1 = m_vertices.row(edge[1]);
    Float length = (v0 - v1).norm();
    if (length < max_length) return;

    bool adj_faces_are_valid = true;
    for (auto fi : adj_faces) {
        adj_faces_are_valid = adj_faces_are_valid && m_face_is_valid[fi];
    }
    if (!adj_faces_are_valid) return;

    const size_t base_v_idx = m_vertices.rows() + m_new_vertices.size();

    std::list<VectorF> edge_vertices = {v0, v1};
    split(edge_vertices, edge_vertices.begin(), length, max_length);
    assert(edge_vertices.size() > 2);

    const auto v_begin = std::next(edge_vertices.begin());
    const auto v_end = std::prev(edge_vertices.end());
    for (auto v_itr = v_begin; v_itr != v_end; v_itr++) {
        m_new_vertices.push_back(*v_itr);
    }

    const size_t num_edge_vertices = edge_vertices.size();
    for (auto fi : adj_faces) {
        const auto& f = m_faces.row(fi);
        size_t opposite_v_id = get_opposite_vertex_index(f, edge[0], edge[1]);
        bool is_correctly_orientated = has_same_orientation(
                f, opposite_v_id, edge[0], edge[1]);
        for (size_t i=0; i<num_edge_vertices-1; i++) {
            size_t curr_idx = i==0 ? edge[0] : i-1 + base_v_idx;
            size_t next_idx = i==num_edge_vertices-2 ? edge[1] : i + base_v_idx;
            if (is_correctly_orientated)
                m_new_faces.emplace_back(opposite_v_id, curr_idx, next_idx);
            else
                m_new_faces.emplace_back(opposite_v_id, next_idx, curr_idx);
        }

        m_face_is_valid[fi] = false;
    }
}

void EdgeSplitter::finalize_geometry() {
    if (m_new_vertices.empty()) {
        assert(m_new_faces.empty());
        return;
    }
    assert(!m_new_faces.empty());

    auto new_vertices = MatrixUtils::rowstack(m_new_vertices);
    m_vertices = MatrixUtils::vstack<MatrixFr>({m_vertices, new_vertices});

    std::vector<Vector3I> faces;
    const size_t num_ori_faces = m_faces.rows();
    for (size_t i=0; i<num_ori_faces; i++) {
        if (m_face_is_valid[i]) {
            faces.emplace_back(m_faces.row(i));
        }
    }

    if (faces.empty()) {
        m_faces = MatrixUtils::rowstack(m_new_faces);
    } else {
        auto ori_faces = MatrixUtils::rowstack(faces);
        auto new_faces = MatrixUtils::rowstack(m_new_faces);
        m_faces = MatrixUtils::vstack<MatrixIr>({ori_faces, new_faces});
    }
}

