/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "MixedMeshTiler.h"
#include <iostream>
#include <sstream>
#include <Core/Exception.h>
#include <Math/MatrixUtils.h>

#include "MeshTilerHelper.h"
using namespace PyMesh;
using namespace MeshTilerHelper;
using namespace MatrixUtils;

MixedMeshTiler::MixedMeshTiler(
        const MixedMeshTiler::Wires& unit_wires, MeshPtr mesh,
        MixedMeshTiler::TargetType target_type,
        MixedMeshTiler::DofType dof_type)
: TilerEngine(NULL), m_mesh(mesh), m_unit_wires(unit_wires),
m_target_type(target_type), m_dof_type(dof_type) {
    if (!m_mesh->has_attribute("pattern_id")) {
        throw RuntimeError(
                "Mesh attribute \"pattern_id\" is required by mixed mesh tiler");
    }

    VectorI pattern_id = m_mesh->get_attribute("pattern_id").cast<int>();
    if (pattern_id.maxCoeff() >= m_unit_wires.size()) {
        std::stringstream err_msg;
        err_msg << "Too few unit patterns supplied: expecting "
            << pattern_id.maxCoeff() << " patterns, but get "
            << m_unit_wires.size() << " patterns";
        throw RuntimeError(err_msg.str());
    }
}

WireNetwork::Ptr MixedMeshTiler::tile() {
    const size_t num_cells = get_num_cells();
    auto transforms = get_tiling_operators();
    auto vars_array = extract_attributes(m_mesh);
    VectorI pattern_id = m_mesh->get_attribute("pattern_id").cast<int>();
    assert(pattern_id.size() == num_cells);

    m_tiled_vertices.clear();
    m_tiled_edges.clear();
    m_tiled_thicknesses.clear();
    m_tiled_offsets.clear();

    size_t v_count = 0;
    auto transform_itr = transforms.begin();
    for (size_t i=0; i<num_cells; i++) {
        set_active_wire_network(pattern_id[i]);
        scale_to_unit_box();
        append_vertices(*transform_itr);
        append_edges(v_count);
        append_thicknesses(vars_array[i]);
        append_offsets(vars_array[i], *transform_itr);

        v_count += m_unit_wire_network->get_num_vertices();
        transform_itr++;
    }

    MatrixFr vertices = vstack(m_tiled_vertices);
    MatrixIr edges = vstack(m_tiled_edges);
    MatrixFr thicknesses = vstack(m_tiled_thicknesses);
    MatrixFr offsets = vstack(m_tiled_offsets);
    assert(edges.minCoeff() >= 0);
    assert(edges.maxCoeff() < vertices.rows());

    WireNetwork::Ptr tiled_network =
        WireNetwork::create_raw(vertices, edges);

    tiled_network->add_attribute("thickness",
            m_target_type == ParameterCommon::VERTEX);
    tiled_network->set_attribute("thickness", thicknesses);
    tiled_network->add_attribute("vertex_offset", true);
    tiled_network->set_attribute("vertex_offset", offsets);

    clean_up(*tiled_network);
    return tiled_network;
}

size_t MixedMeshTiler::get_num_cells() const {
    const size_t dim = m_mesh->get_dim();
    if (dim == 2) {
        return m_mesh->get_num_faces();
    } else if (dim == 3) {
        return m_mesh->get_num_voxels();
    } else {
        throw NotImplementedError("Only 2D and 3D mesh are supported");
    }
}

TilerEngine::FuncList MixedMeshTiler::get_tiling_operators() const {
    const size_t dim = m_mesh->get_dim();
    if (dim == 2) {
        return get_2D_tiling_operators(m_mesh);
    } else if (dim == 3) {
        return get_3D_tiling_operators(m_mesh);
    } else {
        throw NotImplementedError("Only 2D and 3D mesh are supported");
    }
}

void MixedMeshTiler::set_active_wire_network(size_t i) {
    assert(i < m_unit_wires.size());
    m_unit_wire_network = m_unit_wires[i];
    switch (m_dof_type) {
        case ParameterManager::ISOTROPIC:
            m_params = ParameterManager::create_isotropic(
                    m_unit_wire_network, 0.5, m_target_type);
            break;
        case ParameterManager::ORTHOTROPIC:
            m_params = ParameterManager::create(
                    m_unit_wire_network, 0.5, m_target_type);
            break;
        default:
            throw RuntimeError(
                    "Only isotropic and orthotropic dofs are supported in mixed mesh tiling");
    }

    auto& thickness_params = m_params->get_thickness_params();
    auto& offset_params = m_params->get_offset_params();

    size_t param_count = 0;
    for (auto& param : thickness_params) {
        std::stringstream formula;
        formula << "dof_" << param_count;
        param->set_formula(formula.str());
        param_count++;
    }
    for (auto& param : offset_params) {
        std::stringstream formula;
        formula << "dof_" << param_count;
        param->set_formula(formula.str());
        param_count++;
    }
}

void MixedMeshTiler::scale_to_unit_box() {
    if (!m_unit_wire_network) {
        throw RuntimeError("Unit wire network is not initialized");
    }

    const size_t dim = m_mesh->get_dim();
    VectorF cell_size = VectorF::Ones(dim);
    VectorF center = cell_size * 0.5;
    normalize_unit_wire(cell_size);
    m_unit_wire_network->translate(center);
}

void MixedMeshTiler::append_vertices(const Func& f) {
    const MatrixFr& vertices = m_unit_wire_network->get_vertices();
    m_tiled_vertices.emplace_back(f(vertices));
}

void MixedMeshTiler::append_edges(size_t v_count) {
    const MatrixIr& edges = m_unit_wire_network->get_edges();
    m_tiled_edges.emplace_back(edges.array() + v_count);
}

void MixedMeshTiler::append_thicknesses(const MixedMeshTiler::Variables& vars) {
    m_tiled_thicknesses.emplace_back(m_params->evaluate_thickness(vars));
}

void MixedMeshTiler::append_offsets(const MixedMeshTiler::Variables& vars,
        const TilerEngine::Func& f) {
    MatrixFr offset = m_params->evaluate_offset(vars);
    const auto& vertices = m_unit_wire_network->get_vertices();
    m_tiled_offsets.emplace_back(f(vertices + offset) - f(vertices));
}

