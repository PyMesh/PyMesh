/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "LongEdgeRemoval.h"

#include <functional>
#include <iostream>
#include <list>
#include <queue>

#include <Math/MatrixUtils.h>

#include "EdgeSplitter.h"

using namespace PyMesh;

namespace LongEdgeRemovalHelper {
    void split(std::list<VectorF>& end_points,
            std::list<VectorF>::iterator begin_itr,
            Float length, Float threshold) {
        if (length <= threshold) return;

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
}
using namespace LongEdgeRemovalHelper;

void LongEdgeRemoval::run(Float max_length, bool recursive) {
    size_t num_added_triangles = 0;
    do {
        init_edge_map();
        split_long_edges(max_length);
        num_added_triangles = retriangulate();
    } while (recursive && num_added_triangles != 0);
}

void LongEdgeRemoval::init_edge_map() {
    m_edge_map.clear();
    const size_t num_faces = m_faces.rows();
    assert(m_faces.cols() == 3);
    for (size_t i=0; i<num_faces; i++) {
        const auto& f = m_faces.row(i);
        m_edge_map.insert(Triplet(f[0], f[1]), i);
        m_edge_map.insert(Triplet(f[1], f[2]), i);
        m_edge_map.insert(Triplet(f[2], f[0]), i);
    }
}

void LongEdgeRemoval::split_long_edges(Float max_length) {
    const size_t dim = m_vertices.cols();
    const size_t num_ori_vertices = m_vertices.rows();
    size_t vertex_count = num_ori_vertices;
    std::vector<VectorF> new_vertices;
    for (auto& itr : m_edge_map) {
        const auto& edge = itr.first;
        const auto& raw_edge = edge.get_ori_data();
        std::list<VectorF> chain;
        chain.push_back(m_vertices.row(raw_edge[0]));
        chain.push_back(m_vertices.row(raw_edge[1]));
        Float edge_length = (chain.front() - chain.back()).norm();
        split(chain, chain.begin(), edge_length, max_length);

        std::vector<size_t> vertex_indices;
        vertex_indices.push_back(raw_edge[0]);
        const auto begin = std::next(chain.begin());
        const auto end = std::prev(chain.end());
        for (auto itr = begin; itr != end; itr++) {
            new_vertices.push_back(*itr);
            vertex_indices.push_back(vertex_count);
            vertex_count++;
        }
        vertex_indices.push_back(raw_edge[1]);

        itr.second = vertex_indices;
    }

    m_vertices.conservativeResize(vertex_count, dim);
    if (new_vertices.size() > 0) {
        m_vertices.block(num_ori_vertices, 0, 
                vertex_count - num_ori_vertices, dim) =
            MatrixUtils::rowstack(new_vertices);
    }
}

size_t LongEdgeRemoval::retriangulate() {
    const size_t num_faces = m_faces.rows();
    std::vector<VectorI> faces;
    for (size_t i=0; i<num_faces; i++) {
        size_t v0_idx, v1_idx, v2_idx;
        auto chain = get_vertex_chain_around_triangle(
                i, v0_idx, v1_idx, v2_idx);
        if (chain.size() == 3) {
            faces.push_back(m_faces.row(i));
        } else {
            triangulate_chain(faces, chain, v0_idx, v1_idx, v2_idx);
        }
    }
    m_faces = MatrixUtils::rowstack(faces);
    const size_t num_refined_faces = m_faces.rows();
    return num_refined_faces - num_faces;
}

void LongEdgeRemoval::triangulate_chain(
        std::vector<VectorI>& faces,
        const std::vector<size_t>& chain,
        size_t v0_idx, size_t v1_idx, size_t v2_idx) {
    const size_t chain_size = chain.size();
    auto next = [&](size_t i) { return (i+1) % chain_size; };
    auto prev = [&](size_t i) { return (i+chain_size-1) % chain_size; };
    auto length = [&](size_t vi, size_t vj) {
        return (m_vertices.row(vi) - m_vertices.row(vj)).norm();
    };

    MatrixIr visited = MatrixIr::Zero(chain_size, 3);
    visited(v0_idx, 0) = 1;
    visited(v1_idx, 1) = 1;
    visited(v2_idx, 2) = 1;
    MatrixIr candidates(3, 6);
    candidates << v0_idx, next(v0_idx), prev(v0_idx), 0, 0, 0,
                  v1_idx, next(v1_idx), prev(v1_idx), 0, 0, 0,
                  v2_idx, next(v2_idx), prev(v2_idx), 0, 0, 0;
    MatrixFr candidate_lengths(3, 2);
    const Float NOT_USED = std::numeric_limits<Float>::max();
    candidate_lengths
        << length(chain[candidates(0, 1)], chain[candidates(0, 2)]),
           NOT_USED,
           length(chain[candidates(1, 1)], chain[candidates(1, 2)]),
           NOT_USED,
           length(chain[candidates(2, 1)], chain[candidates(2, 2)]),
           NOT_USED;

    auto index_comp = [&](size_t i, size_t j) {
        // Use greater than operator so the queue is a min heap.
        return candidate_lengths.row(i).minCoeff() >
            candidate_lengths.row(j).minCoeff();
    };
    std::priority_queue<size_t, std::vector<size_t>, decltype(index_comp)>
        Q(index_comp);
    Q.push(0);
    Q.push(1);
    Q.push(2);

    while (!Q.empty()) {
        size_t idx = Q.top();
        Q.pop();
        size_t selection;
        if (candidate_lengths(idx, 0) != NOT_USED &&
                candidate_lengths(idx, 0) <= candidate_lengths(idx, 1)) {
            selection = 0;
        } else if (candidate_lengths(idx, 1) != NOT_USED &&
                candidate_lengths(idx, 1) < candidate_lengths(idx, 0)){
            selection = 1;
        } else {
            continue;
        }
        size_t base_v = candidates(idx,  selection * 3 + 0);
        size_t right_v = candidates(idx, selection * 3 + 1);
        size_t left_v = candidates(idx,  selection * 3 + 2);
        assert(visited(base_v, idx) >= 1);
        if (visited.row(base_v).sum() > 1 ||
                visited(right_v, idx) > 1 ||
                visited(left_v, idx) > 1) {
            candidate_lengths(idx, selection) = NOT_USED;
            Q.push(idx);
            continue;
        }

        visited(right_v, idx) = 1;
        visited(left_v, idx) = 1;
        visited(base_v, idx) = 2;
        faces.push_back(Vector3I(chain[base_v], chain[right_v], chain[left_v]));

        if (visited.row(right_v).sum() == 1) {
            size_t right_to_right = next(right_v);
            Float edge_len = length(chain[left_v], chain[right_to_right]);
            candidate_lengths(idx, 0) = edge_len;
            candidates.block(idx, 0, 1, 3) =
                Vector3I(right_v, right_to_right, left_v).transpose();
        } else {
            candidate_lengths(idx, 0) = NOT_USED;
        }
        if (visited.row(left_v).sum() == 1) {
            size_t left_to_left = prev(left_v);
            Float edge_len = length(chain[right_v], chain[left_to_left]);
            candidate_lengths(idx, 1) = edge_len;
            candidates.block(idx, 3, 1, 3) =
                Vector3I(left_v, right_v, left_to_left).transpose();
        } else {
            candidate_lengths(idx, 1) = NOT_USED;
        }
        Q.push(idx);
    }
    auto visited_sum = (visited.array() > 0).rowwise().count().eval();
    if ((visited_sum.array() > 1).count() == 3) {
        Vector3I face;
        size_t count = 0;
        for (size_t i=0; i<chain_size; i++) {
            if (visited_sum[i] > 1) {
                assert(count < 3);
                face[count] = chain[i];
                count++;
            }
        }
        faces.push_back(face);
    }
}

std::vector<size_t> LongEdgeRemoval::get_vertex_chain_around_triangle(
        size_t fi, size_t& v0_idx, size_t& v1_idx, size_t& v2_idx) {
    const auto& f = m_faces.row(fi);
    const auto& chain_01 = m_edge_map[Triplet(f[0], f[1])];
    const auto& chain_12 = m_edge_map[Triplet(f[1], f[2])];
    const auto& chain_20 = m_edge_map[Triplet(f[2], f[0])];

    std::vector<size_t> chain;
    chain.reserve(chain_01.size() + chain_12.size() + chain_20.size() - 3);
    v0_idx = 0;
    chain.push_back(f[0]);
    if (f[0] == chain_01.front()) {
        chain.insert(chain.end(),
                std::next(chain_01.begin()),
                std::prev(chain_01.end()));
    } else {
        assert(f[0] == chain_01.back());
        chain.insert(chain.end(),
                std::next(chain_01.rbegin()),
                std::prev(chain_01.rend()));
    }

    v1_idx = chain.size();
    chain.push_back(f[1]);
    if (f[1] == chain_12.front()) {
        chain.insert(chain.end(),
                std::next(chain_12.begin()),
                std::prev(chain_12.end()));
    } else {
        assert(f[1] == chain_12.back());
        chain.insert(chain.end(),
                std::next(chain_12.rbegin()),
                std::prev(chain_12.rend()));
    }

    v2_idx = chain.size();
    chain.push_back(f[2]);
    if (f[2] == chain_20.front()) {
        chain.insert(chain.end(),
                std::next(chain_20.begin()),
                std::prev(chain_20.end()));
    } else {
        assert(f[2] == chain_20.back());
        chain.insert(chain.end(),
                std::next(chain_20.rbegin()),
                std::prev(chain_20.rend()));
    }
    return chain;
}
