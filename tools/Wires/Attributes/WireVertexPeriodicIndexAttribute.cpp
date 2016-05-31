/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "WireVertexPeriodicIndexAttribute.h"

#include <algorithm>
#include <cmath>
#include <functional>
#include <list>
#include <sstream>

#include <Core/Exception.h>
#include <Wires/WireNetwork/WireNetwork.h>
#include <Misc/HashGrid.h>

using namespace PyMesh;

namespace WireVertexPeriodicIndexAttributeHelper {
    typedef std::list<std::function<VectorF(const VectorF&)> > FunctionList;

    FunctionList get_periodic_transforms(const VectorF& bbox_size) {
        const size_t dim = bbox_size.size();
        FunctionList transforms;
        if (dim == 2) {
            Vector2F step[] = {
                Vector2F(bbox_size[0], 0.0),
                Vector2F(0.0, bbox_size[1]),
                Vector2F(bbox_size[0], bbox_size[1]),
                Vector2F(bbox_size[0],-bbox_size[1])
            };
            transforms.push_back([=](const VectorF& v) { return v+step[0]; });
            transforms.push_back([=](const VectorF& v) { return v+step[1]; });
            transforms.push_back([=](const VectorF& v) { return v+step[2]; });
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
                Vector3F(          0.0, bbox_size[1], bbox_size[2]),
                Vector3F( bbox_size[0], bbox_size[1], bbox_size[2]),
                Vector3F(-bbox_size[0], bbox_size[1], bbox_size[2]),
                Vector3F( bbox_size[0],-bbox_size[1], bbox_size[2]),
                Vector3F( bbox_size[0], bbox_size[1],-bbox_size[2])
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
            transforms.push_back([=](const VectorF& v) { return v+step[9]; });
            transforms.push_back([=](const VectorF& v) { return v+step[10]; });
            transforms.push_back([=](const VectorF& v) { return v+step[11]; });
            transforms.push_back([=](const VectorF& v) { return v+step[12]; });
        } else {
            std::stringstream err_msg;
            err_msg << "Unsupported dimension: " << dim;
            throw NotImplementedError(err_msg.str());
        }

        return transforms;
    }
}

using namespace WireVertexPeriodicIndexAttributeHelper;

void WireVertexPeriodicIndexAttribute::compute(const WireNetwork& network) {
    const Float EPS = 1e-6;
    const size_t dim = network.get_dim();
    const size_t num_vertices = network.get_num_vertices();
    const MatrixFr& vertices = network.get_vertices();

    const VectorF bbox_min = vertices.colwise().minCoeff();
    const VectorF bbox_max = vertices.colwise().maxCoeff();
    const VectorF bbox_size = bbox_max - bbox_min;

    HashGrid::Ptr grid = HashGrid::create(EPS, dim);
    m_values.resize(num_vertices, 1);
    for (size_t i=0; i<num_vertices; i++) {
        const VectorF& v = vertices.row(i);
        m_values(i, 0) = i;
        grid->insert(i, v);
    }

    FunctionList transforms = get_periodic_transforms(bbox_size);
    for (auto trans: transforms) {
        std::vector<bool> visited(num_vertices, false);
        for (size_t i=0; i<num_vertices; i++) {
            if (visited[i]) continue;
            const VectorF& v = vertices.row(i);

            if ((v - bbox_min).cwiseAbs().minCoeff() < EPS) {
                VectorF opposite = trans(v);
                VectorI opposite_indices = grid->get_items_near_point(opposite);
                VectorI current_indices = grid->get_items_near_point(v);

                if (opposite_indices.size() == 0) continue;

                std::vector<int> indices(opposite_indices.size() +
                        current_indices.size());
                std::copy(opposite_indices.data(), opposite_indices.data() +
                        opposite_indices.size(), indices.begin());
                std::copy(current_indices.data(), current_indices.data() +
                        current_indices.size(), indices.begin() +
                        opposite_indices.size());

                auto get_value = [&](size_t i) {
                    return size_t(m_values(i,0));
                };

                std::vector<size_t> periodic_indices(indices.size());
                std::transform(indices.begin(), indices.end(),
                        periodic_indices.begin(), get_value);
                size_t label = *std::min_element(
                        periodic_indices.begin(), periodic_indices.end());

                for (auto idx : indices) {
                    m_values(idx, 0) = label;
                    visited[idx] = true;
                }
            }
        }
    }
}

