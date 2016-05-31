/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "CGALConvexHull.h"

#include <cassert>
#include <map>

using namespace PyMesh;

namespace CGALConvexHullHelper {
    struct VectorFComp {
        bool operator()(const VectorF& first, const VectorF& second) const {
            assert(first.size() == second.size());
            const size_t dim = first.size();
            for (size_t i=0; i<dim; i++) {
                if (first[i] < second[i]) return true;
                if (first[i] > second[i]) return false;
            }
            return false;
        }
    };
}

using namespace CGALConvexHullHelper;

void CGALConvexHull::compute_index_map(const MatrixFr& points) {
    const size_t num_pts = points.rows();
    std::map<VectorF, size_t, VectorFComp> vertex_map;
    for (size_t i=0; i<num_pts; i++) {
        vertex_map.insert(std::pair<VectorF, size_t>(points.row(i), i));
    }

    const size_t num_vertices = m_vertices.rows();
    m_index_map.resize(num_vertices);
    for (size_t i=0; i<num_vertices; i++) {
        auto itr = vertex_map.find(m_vertices.row(i));
        assert(itr != vertex_map.end());
        m_index_map[i] = itr->second;
    }
}

