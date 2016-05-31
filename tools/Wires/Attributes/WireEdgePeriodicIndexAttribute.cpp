/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "WireEdgePeriodicIndexAttribute.h"

#include <functional>
#include <list>

#include <Misc/Triplet.h>
#include <Misc/TripletMap.h>
#include <Misc/HashGrid.h>
#include <Wires/WireNetwork/WireNetwork.h>

using namespace PyMesh;

namespace WireEdgePeriodicIndexAttributeHelper {
    typedef std::list<std::function<VectorF(const VectorF&)> > FunctionList;

    FunctionList get_periodic_transforms(const VectorF& bbox_size) {
        const size_t dim = bbox_size.size();
        FunctionList transforms;
        if (dim == 2) {
            Vector2F step[] = {
                Vector2F(bbox_size[0], 0.0),
                Vector2F(0.0, bbox_size[1])
            };
            transforms.push_back([=](const VectorF& v) { return v+step[0]; });
            transforms.push_back([=](const VectorF& v) { return v+step[1]; });
        } else if (dim == 3) {
            Vector3F step[] = {
                Vector3F( bbox_size[0],          0.0,          0.0),
                Vector3F(          0.0, bbox_size[1],          0.0),
                Vector3F(          0.0,          0.0, bbox_size[2]),
                Vector3F( bbox_size[0], bbox_size[1],          0.0),
                Vector3F(-bbox_size[0], bbox_size[1],          0.0),
                Vector3F( bbox_size[0],          0.0,-bbox_size[2]),
                Vector3F( bbox_size[0],          0.0, bbox_size[2]),
                Vector3F(          0.0,-bbox_size[1], bbox_size[2]),
                Vector3F(          0.0, bbox_size[1], bbox_size[2])
            };
            transforms.push_back([=](const VectorF& v) { return v+step[0]; });
            transforms.push_back([=](const VectorF& v) { return v+step[1]; });
            transforms.push_back([=](const VectorF& v) { return v+step[2]; });
            transforms.push_back([=](const VectorF& v) { return v+step[3]; });
            transforms.push_back([=](const VectorF& v) { return v+step[4]; });
            transforms.push_back([=](const VectorF& v) { return v+step[5]; });
            transforms.push_back([=](const VectorF& v) { return v+step[6]; });
            transforms.push_back([=](const VectorF& v) { return v+step[7]; });
            transforms.push_back([=](const VectorF& v) { return v+step[8]; });
        } else {
            std::stringstream err_msg;
            err_msg << "Unsupported dimension: " << dim;
            throw NotImplementedError(err_msg.str());
        }

        return transforms;
    }

}

using namespace WireEdgePeriodicIndexAttributeHelper;

void WireEdgePeriodicIndexAttribute::compute(const WireNetwork& network) {
    const Float EPS = 1e-6;
    const size_t num_vertices = network.get_num_vertices();
    const size_t num_edges = network.get_num_edges();
    const MatrixFr& vertices = network.get_vertices();
    const MatrixIr& edges = network.get_edges();

    const VectorF bbox_min = vertices.colwise().minCoeff();
    const VectorF bbox_max = vertices.colwise().maxCoeff();
    const VectorF bbox_size = bbox_max - bbox_min;

    std::vector<bool> on_min_border(num_vertices, false);
    HashGrid::Ptr grid = HashGrid::create(EPS, network.get_dim());
    for (size_t i=0; i<num_vertices; i++) {
        const VectorF& v = vertices.row(i);
        grid->insert(i, v);
        if ((v-bbox_min).cwiseAbs().minCoeff() < EPS) {
            on_min_border[i] = true;
        }
    }

    m_values.resize(num_edges, 1);
    TripletMap<size_t> edge_map;
    for (size_t i=0; i<num_edges; i++) {
        const VectorI& edge = edges.row(i);
        Triplet trip(edge[0], edge[1]);
        edge_map.insert(trip, i);
        m_values(i, 0) = i;
    }

    FunctionList transforms = get_periodic_transforms(bbox_size);

    for (auto transform : transforms) {
        for (size_t i=0; i<num_edges; i++) {
            const VectorI& edge = edges.row(i);
            if (on_min_border[edge[0]] || on_min_border[edge[1]]) {
                VectorF v0 = transform(vertices.row(edge[0]));
                VectorF v1 = transform(vertices.row(edge[1]));

                VectorI opposite_v0_index = grid->get_items_near_point(v0);
                VectorI opposite_v1_index = grid->get_items_near_point(v1);
                if (opposite_v0_index.size() != 1 ||
                        opposite_v1_index.size() != 1) { continue; }

                Triplet trip(opposite_v0_index[0], opposite_v1_index[0]);
                auto itr = edge_map.find(trip);
                if (itr != edge_map.end()) {
                    if (itr->second.size() != 1) {
                        throw RuntimeError("Duplicated edge detected");
                    }
                    size_t j = itr->second[0];
                    size_t label = std::min(m_values(i, 0), m_values(j, 0));
                    m_values(i, 0) = label;
                    m_values(j, 0) = label;
                }
            }
        }
    }
}

