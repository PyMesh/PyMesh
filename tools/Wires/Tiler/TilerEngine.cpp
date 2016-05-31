/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "TilerEngine.h"

#include <cassert>
#include <iostream>
#include <sstream>
#include <unordered_set>
#include <vector>

#include <Core/Exception.h>
#include <MeshUtils/DuplicatedVertexRemoval.h>
#include <Misc/Triplet.h>

using namespace PyMesh;

namespace TilerEngineHelper {
    struct hash {
        size_t operator()(const Triplet& value) const {
            return value.hash();
        }
    };

    std::vector<bool> create_duplication_mask(const MatrixIr& edges) {
        const size_t num_edges = edges.rows();

        std::unordered_set<Triplet, hash> unique_set;
        std::vector<bool> mask(num_edges, false);

        for (size_t i=0; i<num_edges; i++) {
            const auto& edge = edges.row(i);
            Triplet key(edge[0], edge[1]);
            auto itr = unique_set.find(key);
            if (itr == unique_set.end()) {
                unique_set.insert(key);
            } else {
                mask[i] = true;
            }
        }

        return mask;
    }

    template <typename T>
    T filter(const T& data, const std::vector<bool>& mask) {
        const size_t num_rows = data.rows();
        size_t valid_rows = 0;
        for (auto itr : mask) {
            if (!itr) valid_rows++;
        }

        T result(valid_rows, data.cols());
        size_t count = 0;
        for (size_t i=0; i<num_rows; i++) {
            if (mask[i]) continue;
            result.row(count) = data.row(i);
            count++;
        }

        return result;
    }
}

using namespace TilerEngineHelper;

void TilerEngine::with_parameters(ParameterManager::Ptr params) {
    assert(params);
    m_params = params;
    m_params->set_wire_network(m_unit_wire_network);
}

MatrixFr TilerEngine::tile_vertices(const TilerEngine::FuncList& funcs) {
    const size_t num_copies = funcs.size();
    const size_t dim = m_unit_wire_network->get_dim();
    const size_t num_vertices = m_unit_wire_network->get_num_vertices();
    const MatrixFr& vertices = m_unit_wire_network->get_vertices();

    MatrixFr tiled_vertices(num_copies * num_vertices, dim);

    size_t vertex_count = 0;
    for (auto f : funcs) {
        tiled_vertices.block(vertex_count, 0, num_vertices, dim) = f(vertices);
        vertex_count += num_vertices;
    }

    return tiled_vertices;
}

MatrixIr TilerEngine::tile_edges(size_t num_repetitions) {
    const size_t num_vertices = m_unit_wire_network->get_num_vertices();
    const size_t num_edges    = m_unit_wire_network->get_num_edges();

    MatrixIr tiled_edges(num_edges * num_repetitions, 2);

    size_t vertex_count = 0;
    for (size_t i=0; i<num_repetitions; i++) {
        tiled_edges.block(num_edges * i, 0, num_edges, 2) =
            m_unit_wire_network->get_edges().array() + vertex_count;
        vertex_count += num_vertices;
    }
    return tiled_edges;
}

void TilerEngine::normalize_unit_wire(const VectorF& cell_size) {
    if (cell_size.minCoeff() <= 1e-30) {
        const size_t dim = cell_size.size();
        if (dim == 3)
            throw RuntimeError("It seems the 3D wires are flat.");
        else if (dim == 2)
            throw RuntimeError("It seems the 2D wires are degenerated/linear.");
        else
            throw NotImplementedError("Unsupported dimension!");
    }

    VectorF factors = cell_size.cwiseQuotient(
            m_unit_wire_network->get_bbox_max() - m_unit_wire_network->get_bbox_min());
    m_unit_wire_network->center_at_origin();
    m_unit_wire_network->scale(factors);
}

void TilerEngine::update_attributes(WireNetwork& wire_network, size_t num_repetitions) {
    std::vector<std::string> attr_names = m_unit_wire_network->get_attribute_names();
    for (auto itr : attr_names) {
        const std::string& name = itr;
        wire_network.add_attribute(name,
                m_unit_wire_network->is_vertex_attribute(name), false);

        const MatrixFr& values = m_unit_wire_network->get_attribute(name);
        const size_t rows = values.rows();
        const size_t cols = values.cols();
        MatrixFr tiled_values(rows * num_repetitions, cols);

        for (size_t i=0; i<num_repetitions; i++) {
            tiled_values.block(i*rows, 0, rows, cols) = values;
        }

        wire_network.set_attribute(name, tiled_values);
    }
}

void TilerEngine::clean_up(WireNetwork& wire_network, Float tol) {
    remove_duplicated_vertices(wire_network, tol);
    remove_duplicated_edges(wire_network);
}

void TilerEngine::remove_duplicated_vertices(WireNetwork& wire_network, Float tol) {
    const size_t num_input_vertices = wire_network.get_num_vertices();

    DuplicatedVertexRemoval remover(wire_network.get_vertices(), wire_network.get_edges());
    remover.run(tol);
    MatrixFr vertices = remover.get_vertices();
    MatrixIr edges = remover.get_faces();
    VectorI index_map = remover.get_index_map();
    assert(num_input_vertices == index_map.size());

    wire_network.set_vertices(vertices);
    wire_network.set_edges(edges);

    const size_t num_output_vertices = wire_network.get_num_vertices();
    std::vector<std::string> attr_names = wire_network.get_attribute_names();
    for (auto itr : attr_names) {
        const std::string& name = itr;
        if (wire_network.is_vertex_attribute(name)) {
            MatrixFr values = wire_network.get_attribute(name);
            MatrixFr updated_values = MatrixFr::Zero(num_output_vertices, values.cols());
            VectorF count = VectorF::Zero(num_output_vertices);
            for (size_t i=0; i<num_input_vertices; i++) {
                size_t j = index_map[i];
                updated_values.row(j) += values.row(i);
                count[j] += 1;
            }

            for (size_t i=0; i<num_output_vertices; i++) {
                assert(count[i] > 0);
                updated_values.row(i) /= count[i];
            }
            wire_network.set_attribute(name, updated_values);
        }
    }
}

void TilerEngine::remove_duplicated_edges(WireNetwork& wire_network) {
    const size_t num_edges = wire_network.get_num_edges();

    const MatrixIr& edges = wire_network.get_edges();
    std::vector<bool> mask = create_duplication_mask(edges);

    MatrixIr updated_edges = filter(edges, mask);
    wire_network.set_edges(updated_edges);

    std::vector<std::string> attr_names = wire_network.get_attribute_names();
    for (auto itr : attr_names) {
        const std::string& name = itr;
        if (!wire_network.is_vertex_attribute(name)) {
            MatrixFr values = wire_network.get_attribute(name);
            MatrixFr updated_values = filter(values, mask);
            wire_network.set_attribute(name, updated_values);
        }
    }
}

