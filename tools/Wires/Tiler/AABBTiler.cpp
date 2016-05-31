/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "AABBTiler.h"

#include <cassert>
#include <iostream>
#include <list>

#include <Wires/Parameters/ParameterCommon.h>

using namespace PyMesh;

namespace AABBTilerHelper {
    std::vector<VectorI> enumerate(const VectorI& repetitions) {
        std::vector<VectorI> result;
        const size_t dim = repetitions.size();
        if (dim == 2) {
            for (size_t i=0; i<repetitions[0]; i++) {
                for (size_t j=0; j<repetitions[1]; j++) {
                    result.push_back(Vector2I(i,j));
                }
            }
        } else if (dim == 3) {
            for (size_t i=0; i<repetitions[0]; i++) {
                for (size_t j=0; j<repetitions[1]; j++) {
                    for (size_t k=0; k<repetitions[2]; k++) {
                        result.push_back(Vector3I(i,j,k));
                    }
                }
            }
        } else {
            std::stringstream err_msg;
            err_msg << "Unsupported dim: " << dim;
            throw NotImplementedError(err_msg.str());
        }

        return result;
    }

    template<typename T>
    void run_compatibility_check(size_t dim, const T& data) {
        if (data.size() != dim) {
            std::stringstream err_msg;
            err_msg << "Imcompatible data dimention: " << data.size() << std::endl;
            err_msg << "Expect " << dim << std::endl;
            throw RuntimeError(err_msg.str());
        }
    }

    std::list<std::function<MatrixFr(const MatrixFr&)> > get_tiling_operators(
            const VectorF& ref_pt,
            const VectorF& cell_size,
            const std::vector<VectorI>& indices) {

        std::list<std::function<MatrixFr(const MatrixFr&)> > operators;
        for (auto index : indices) {
            VectorF cur_pt = cell_size.cwiseProduct(index.cast<Float>());
            VectorF offset = cur_pt - ref_pt;
            operators.push_back(
                    [=] (const MatrixFr& vertices) {
                        MatrixFr result(vertices);
                        result.rowwise() += offset.transpose();
                        return result;
                    });
        }
        return operators;
    }
}

using namespace AABBTilerHelper;

AABBTiler::AABBTiler(
        WireNetwork::Ptr unit_wire_network,
        const VectorF& bbox_min,
        const VectorF& bbox_max,
        const VectorI& repetitions)
: TilerEngine(unit_wire_network),
    m_bbox_min(bbox_min),
    m_bbox_max(bbox_max),
    m_repetitions(repetitions) {
        const size_t dim = m_unit_wire_network->get_dim();
        run_compatibility_check(dim, m_bbox_min);
        run_compatibility_check(dim, m_bbox_max);
        run_compatibility_check(dim, m_repetitions);
}

WireNetwork::Ptr AABBTiler::tile() {
    const size_t dim = m_unit_wire_network->get_dim();

    VectorF cell_size = (m_bbox_max - m_bbox_min).cwiseQuotient(
            m_repetitions.cast<Float>());
    normalize_unit_wire(cell_size);

    std::vector<VectorI> indices = enumerate(m_repetitions);
    const size_t num_repetitions = indices.size();
    auto transforms = get_tiling_operators(m_unit_wire_network->get_bbox_min(),
            cell_size, indices);
    MatrixFr tiled_vertices = tile_vertices(transforms);
    MatrixIr tiled_edges = tile_edges(num_repetitions);

    WireNetwork::Ptr tiled_network =
        WireNetwork::create_raw(tiled_vertices, tiled_edges);
    update_attributes(*tiled_network, num_repetitions);
    evaluate_parameters(*tiled_network, transforms);
    clean_up(*tiled_network, 1e-3);
    return tiled_network;
}

void AABBTiler::evaluate_parameters(WireNetwork& wire_network,
        const AABBTiler::FuncList& funcs) {
    evaluate_thickness_parameters(wire_network, funcs);
    evaluate_offset_parameters(wire_network, funcs);
}

void AABBTiler::evaluate_thickness_parameters(WireNetwork& wire_network,
        const AABBTiler::FuncList& funcs) {
    assert(m_params);
    const size_t dim = wire_network.get_dim();
    const size_t num_vertices = wire_network.get_num_vertices();
    const size_t num_edges = wire_network.get_num_edges();
    const size_t num_unit_vertices = m_unit_wire_network->get_num_vertices();
    const size_t num_unit_edges = m_unit_wire_network->get_num_edges();

    size_t num_thickness_entries, thickness_index_stride;
    if (m_params->get_thickness_type() == ParameterCommon::VERTEX) {
        thickness_index_stride = num_unit_vertices;
        num_thickness_entries = num_vertices;
        wire_network.add_attribute("thickness", true);
    } else {
        thickness_index_stride = num_unit_edges;
        num_thickness_entries = num_edges;
        wire_network.add_attribute("thickness", false);
    }
    MatrixFr thickness(num_thickness_entries, 1);

    ParameterCommon::Variables vars;
    size_t count=0;
    for (auto f : funcs) {
        VectorF local_thickness = m_params->evaluate_thickness(vars);
        thickness.block(count * thickness_index_stride, 0,
                thickness_index_stride, 1) = local_thickness;
        count++;
    }
    wire_network.set_attribute("thickness", thickness);
}

void AABBTiler::evaluate_offset_parameters(WireNetwork& wire_network,
        const AABBTiler::FuncList& funcs) {
    const size_t dim = wire_network.get_dim();
    const size_t num_vertices = wire_network.get_num_vertices();
    const size_t num_unit_vertices = m_unit_wire_network->get_num_vertices();

    wire_network.add_attribute("vertex_offset", true);
    MatrixFr attr_value(num_vertices, dim);

    ParameterCommon::Variables vars;
    const MatrixFr& ori_vertices = m_unit_wire_network->get_vertices();
    size_t count=0;
    for (auto f : funcs) {
        MatrixFr local_offseted_vertices = ori_vertices + m_params->evaluate_offset(vars);
        attr_value.block(count * num_unit_vertices, 0,
                num_unit_vertices, dim) = f(local_offseted_vertices);
        count++;
    }

    attr_value = attr_value - wire_network.get_vertices();
    wire_network.set_attribute("vertex_offset", attr_value);
}

