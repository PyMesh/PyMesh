/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "EdgeUtils.h"

#include <sstream>
#include <unordered_map>

#include <Core/Exception.h>
#include <Math/MatrixUtils.h>

using namespace PyMesh;

namespace EdgeUtilsHelper {
    typedef std::unordered_map<size_t, size_t> IndexHash;

    IndexHash compute_next_map(const MatrixIr& edges) {
        IndexHash next;
        const size_t num_edges = edges.rows();
        for (size_t i=0; i<num_edges; i++) {
            const auto& e = edges.row(i);
            next[e[0]] = e[1];
        }
        return next;
    }

    IndexHash compute_valance(const MatrixIr& edges) {
        IndexHash valance;
        const size_t num_edges = edges.rows();
        for (size_t i=0; i<num_edges; i++) {
            const auto& e = edges.row(i);

            auto e0_itr = valance.find(e[0]);
            if (e0_itr == valance.end()) {
                valance[e[0]] = 1;
            } else {
                e0_itr->second++;
            }

            auto e1_itr = valance.find(e[1]);
            if (e1_itr == valance.end()) {
                valance[e[1]] = 1;
            } else {
                e1_itr->second++;
            }
        }

        return valance;
    }

    /**
     * Trace vertices according to the "next" map.  Remove entries in the map
     * once a link has been traced.
     */
    VectorI trace(std::unordered_map<size_t, size_t>& next, size_t start) {
        std::vector<size_t> chain;
        chain.push_back(start);
        while (true) {
            auto itr = next.find(chain.back());
            if (itr == next.end()) break;
            size_t vid = itr->second;
            next.erase(itr);

            if (vid == start) break;
            chain.push_back(vid);
        }
        return MatrixUtils::std2eigen(chain).cast<int>();
    }

    void trace_vertex_chains(const IndexHash& valance, IndexHash& next,
            std::vector<VectorI>& chains) {
        for (auto target_valance : {1, 2}) {
            for (auto item : valance) {
                if (next.find(item.first) == next.end()) continue;
                if (item.second == target_valance) {
                    chains.push_back(trace(next, item.first));
                    const auto& chain = chains.back();
                    const size_t chain_length = chain.size();
                }
            }
        }
    }

    void assert_simple_edge_chains(
            const std::unordered_map<size_t, size_t>& valance) {
        for (auto item : valance) {
            if (item.second != 1 && item.second != 2) {
                std::stringstream err_msg;
                err_msg << "Complex edge loop detected!  ";
                err_msg << "Vertex " << item.first << " has valance " << item.second
                    << std::endl;
                throw RuntimeError(err_msg.str());
            }
        }
    }
}
using namespace EdgeUtilsHelper;

std::vector<VectorI> EdgeUtils::chain_edges(const MatrixIr& edges) {
    std::vector<VectorI> chains;
    IndexHash next = compute_next_map(edges);
    IndexHash valance = compute_valance(edges);
    assert_simple_edge_chains(valance);
    trace_vertex_chains(valance, next, chains);
    return chains;
}

TripletMap<size_t> EdgeUtils::compute_edge_face_adjacency(const MatrixIr& faces) {
    TripletMap<size_t> adjacency;
    const size_t num_faces = faces.rows();
    const size_t vertex_per_face = faces.cols();
    for (size_t i=0; i<num_faces; i++) {
        const auto& f = faces.row(i);
        for (size_t j=0; j<vertex_per_face; j++) {
            adjacency.insert(Triplet(f[j], f[(j+1)%vertex_per_face]), i);
        }
    }
    return adjacency;
}

