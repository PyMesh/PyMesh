/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "MeshTiler.h"

#include <Core/Exception.h>
#include <Wires/Misc/BilinearInterpolation.h>
#include <Wires/Misc/TrilinearInterpolation.h>
#include <Wires/Parameters/ParameterCommon.h>

#include "MeshTilerHelper.h"
using namespace PyMesh;
using namespace MeshTilerHelper;

MeshTiler::MeshTiler(WireNetwork::Ptr unit_wire_network, MeshTiler::MeshPtr mesh) :
    TilerEngine(unit_wire_network), m_mesh(mesh) {
    if (m_mesh->get_dim() != m_unit_wire_network->get_dim()) {
        std::stringstream err_msg;
        err_msg << "Unsupported dim: " << m_mesh->get_dim()
            << ", expect " << m_unit_wire_network->get_dim();
        throw RuntimeError(err_msg.str());
    }
}

WireNetwork::Ptr MeshTiler::tile() {
    const size_t dim = m_mesh->get_dim();
    switch (dim) {
        case 2:
            return tile_2D();
        case 3:
            return tile_3D();
        default:
            std::stringstream err_msg;
            err_msg << "Unsupported dim: " << dim;
            throw NotImplementedError(err_msg.str());
    }
}

WireNetwork::Ptr MeshTiler::tile_2D() {
    const size_t num_cells = m_mesh->get_num_faces();
    scale_to_unit_box();
    auto transforms = get_2D_tiling_operators(m_mesh);
    MatrixFr tiled_vertices = tile_vertices(transforms);
    MatrixIr tiled_edges = tile_edges(num_cells);

    WireNetwork::Ptr tiled_network = WireNetwork::create_raw(tiled_vertices, tiled_edges);
    update_attributes(*tiled_network, num_cells);
    evaluate_parameters(*tiled_network, transforms);
    clean_up(*tiled_network);
    return tiled_network;
}

WireNetwork::Ptr MeshTiler::tile_3D() {
    const size_t num_cells = m_mesh->get_num_voxels();
    scale_to_unit_box();
    auto transforms = get_3D_tiling_operators(m_mesh);
    MatrixFr tiled_vertices = tile_vertices(transforms);
    MatrixIr tiled_edges = tile_edges(num_cells);

    WireNetwork::Ptr tiled_network = WireNetwork::create_raw(tiled_vertices, tiled_edges);
    update_attributes(*tiled_network, num_cells);
    evaluate_parameters(*tiled_network, transforms);
    clean_up(*tiled_network);
    return tiled_network;
}

void MeshTiler::scale_to_unit_box() {
    const size_t dim = m_mesh->get_dim();
    VectorF cell_size = VectorF::Ones(dim);
    VectorF center = cell_size * 0.5;
    normalize_unit_wire(cell_size);
    m_unit_wire_network->translate(center);
}

void MeshTiler::evaluate_parameters(WireNetwork& wire_network,
        const MeshTiler::FuncList& funcs) {
    evaluate_thickness_parameters(wire_network, funcs);
    evaluate_offset_parameters(wire_network, funcs);
}

void MeshTiler::evaluate_thickness_parameters(WireNetwork& wire_network,
        const MeshTiler::FuncList& funcs) {
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

    auto vars_array = extract_attributes(m_mesh);
    assert(vars_array.size() == funcs.size());
    size_t count=0;
    auto vars_itr = vars_array.begin();
    for (auto f : funcs) {
        assert(vars_itr != vars_array.end());
        VectorF local_thickness = m_params->evaluate_thickness(*vars_itr);
        thickness.block(count * thickness_index_stride, 0,
                thickness_index_stride, 1) = local_thickness;
        count++;
        vars_itr++;
    }
    wire_network.set_attribute("thickness", thickness);
}

void MeshTiler::evaluate_offset_parameters(WireNetwork& wire_network,
        const MeshTiler::FuncList& funcs) {
    const size_t dim = wire_network.get_dim();
    const size_t num_vertices = wire_network.get_num_vertices();
    const size_t num_unit_vertices = m_unit_wire_network->get_num_vertices();

    wire_network.add_attribute("vertex_offset", true);
    MatrixFr attr_value(num_vertices, dim);

    auto vars_array = extract_attributes(m_mesh);
    const MatrixFr& ori_vertices = m_unit_wire_network->get_vertices();
    size_t count=0;
    auto vars_itr = vars_array.begin();
    for (auto f : funcs) {
        assert(vars_itr != vars_array.end());
        MatrixFr local_offseted_vertices = ori_vertices +
            m_params->evaluate_offset(*vars_itr);
        attr_value.block(count * num_unit_vertices, 0,
                num_unit_vertices, dim) = f(local_offseted_vertices);
        count++;
        vars_itr++;
    }

    attr_value = attr_value - wire_network.get_vertices();
    wire_network.set_attribute("vertex_offset", attr_value);
}

