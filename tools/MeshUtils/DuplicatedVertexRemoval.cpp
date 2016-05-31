/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "DuplicatedVertexRemoval.h"
#include <cassert>
#include <sstream>
#include <vector>

#include <Core/Exception.h>
#include <Misc/HashGrid.h>

using namespace PyMesh;

DuplicatedVertexRemoval::DuplicatedVertexRemoval(const MatrixFr& vertices, const MatrixIr& faces):
    m_vertices(vertices), m_faces(faces) {
        m_importance_level = VectorI::Zero(m_vertices.rows());
    }

size_t DuplicatedVertexRemoval::run(Float tol) {
    const size_t dim = m_vertices.cols();
    HashGrid::Ptr grid = HashGrid::create(tol, dim);
    const size_t num_vertices = m_vertices.rows();
    const size_t num_faces = m_faces.rows();
    const size_t vertex_per_face = m_faces.cols();
    m_index_map.resize(num_vertices);
    std::vector<size_t> source_index;

    size_t count = 0;
    size_t num_duplications = 0;
    for (size_t i=0; i<num_vertices; i++) {
        int curr_importance_level = m_importance_level[i];
        if (curr_importance_level < 0) {
            m_index_map[i] = count;
            source_index.push_back(i);
            count++;
            continue;
        }
        const VectorF& v = m_vertices.row(i);
        VectorI candidates = grid->get_items_near_point(v);
        const size_t num_candidates = candidates.size();
        if (num_candidates > 0) {
            VectorF dists(num_candidates);
            for (size_t j=0; j<num_candidates; j++) {
                dists[j] = (m_vertices.row(candidates[j]) - v.transpose()).norm();
            }
            size_t min_idx;
            Float min_dist = dists.minCoeff(&min_idx);
            if (min_dist < tol) {
                size_t best_match_idx = candidates[min_idx];
                size_t output_idx = m_index_map[best_match_idx];
                m_index_map[i] = output_idx;

                int matched_importance_level =
                    m_importance_level[source_index[output_idx]];
                if (curr_importance_level > matched_importance_level) {
                    source_index[output_idx] = i;
                }

                num_duplications++;
                continue;
            }
        }

        // No match, add this vertex in the book.
        grid->insert(i, v);
        m_index_map[i] = count;
        source_index.push_back(i);
        count++;
    }

    assert(source_index.size() == count);
    MatrixFr vertices(count, dim);
    for (size_t i=0; i<count; i++) {
        assert(m_index_map[source_index[i]] == i);
        vertices.row(i) = m_vertices.row(source_index[i]);
    }
    m_vertices = vertices;

    for (size_t i=0; i<num_faces; i++) {
        for (size_t j=0; j<vertex_per_face; j++) {
            size_t v_index = m_faces(i,j);
            m_faces(i,j) = m_index_map[v_index];
        }
    }
    return num_duplications;
}
