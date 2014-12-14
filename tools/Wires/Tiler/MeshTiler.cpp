#include "MeshTiler.h"

#include <Core/Exception.h>
#include <Wires/Misc/BilinearInterpolation.h>
#include <Wires/Misc/TrilinearInterpolation.h>
#include <Wires/Parameters/ParameterCommon.h>

namespace MeshTilerHelper {
    TilerEngine::FuncList get_2D_tiling_operators(MeshTiler::MeshPtr mesh) {
        const size_t num_cells = mesh->get_num_faces();
        const size_t num_vertex_per_cell = mesh->get_vertex_per_face();
        if (num_vertex_per_cell != 4) {
            throw NotImplementedError("Only quad guide mesh is supported in 2D");
        }

        TilerEngine::FuncList operators;
        for (size_t i=0; i<num_cells; i++) {
            const VectorI face = mesh->get_face(i);
            MatrixFr corners(num_vertex_per_cell, 2);
            for (size_t j=0; j<num_vertex_per_cell; j++) {
                corners.row(j) = mesh->get_vertex(face[j]).transpose();
            }

            operators.push_back(
                    [=](const MatrixFr& vertices) {
                        BilinearInterpolation interpolator(corners);
                        return interpolator.interpolate_batch(vertices);
                    });
        }
        return operators;
    }

    TilerEngine::FuncList get_3D_tiling_operators(MeshTiler::MeshPtr mesh) {
        const size_t num_cells = mesh->get_num_voxels();
        const size_t num_vertex_per_cell = mesh->get_vertex_per_voxel();
        if (num_vertex_per_cell != 8) {
            throw NotImplementedError("Only hex guide mesh is supported in 3D");
        }

        TilerEngine::FuncList operators;
        for (size_t i=0; i<num_cells; i++) {
            const VectorI voxel = mesh->get_voxel(i);
            MatrixFr corners(num_vertex_per_cell, 3);
            for (size_t j=0; j<num_vertex_per_cell; j++) {
                corners.row(j) = mesh->get_vertex(voxel[j]).transpose();
            }

            operators.push_back(
                    [=](const MatrixFr& vertices) {
                        TrilinearInterpolation interpolator(corners);
                        return interpolator.interpolate_batch(vertices);
                    });
        }
        return operators;
    }

    std::list<ParameterCommon::Variables> extract_face_attributes(
            MeshTiler::MeshPtr mesh) {
        const size_t num_faces = mesh->get_num_faces();
        std::list<ParameterCommon::Variables> vars_list;
        const auto& attr_names = mesh->get_attribute_names();

        for (size_t i=0; i<num_faces; i++) {
            ParameterCommon::Variables vars;
            for (const auto& name : attr_names) {
                const MatrixFr& attr = mesh->get_attribute(name);
                if (attr.rows() != num_faces) continue;
                if (attr.cols() != 1) continue;
                vars[name] = attr(i, 0);
            }
            vars_list.push_back(vars);
        }
        return vars_list;
    }

    std::list<ParameterCommon::Variables> extract_voxel_attributes(
            MeshTiler::MeshPtr mesh) {
        const size_t num_voxels = mesh->get_num_voxels();
        std::list<ParameterCommon::Variables> vars_list;
        const auto& attr_names = mesh->get_attribute_names();

        for (size_t i=0; i<num_voxels; i++) {
            ParameterCommon::Variables vars;
            for (const auto& name : attr_names) {
                const MatrixFr& attr = mesh->get_attribute(name);
                if (attr.rows() != num_voxels) continue;
                if (attr.cols() != 1) continue;
                vars[name] = attr(i, 0);
            }
            vars_list.push_back(vars);
        }
        return vars_list;
    }

    std::list<ParameterCommon::Variables> extract_attributes(
            MeshTiler::MeshPtr mesh) {
        const size_t dim = mesh->get_dim();

        if (dim == 2) {
            return extract_face_attributes(mesh);
        } else if (dim == 3) {
            return extract_voxel_attributes(mesh);
        } else {
            std::stringstream err_msg;
            err_msg << "Unsupport dim: " << dim;
            throw RuntimeError(err_msg.str());
        }
    }
}

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

    auto vars_list = extract_attributes(m_mesh);
    assert(vars_list.size() == funcs.size());
    size_t count=0;
    auto vars_itr = vars_list.begin();
    for (auto f : funcs) {
        assert(vars_itr != vars_list.end());
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

    auto vars_list = extract_attributes(m_mesh);
    const MatrixFr& ori_vertices = m_unit_wire_network->get_vertices();
    size_t count=0;
    auto vars_itr = vars_list.begin();
    for (auto f : funcs) {
        assert(vars_itr != vars_list.end());
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

