#include "MeshTiler.h"

#include <Core/Exception.h>
#include <Wires/Misc/BilinearInterpolation.h>
#include <Wires/Misc/TrilinearInterpolation.h>

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
    MatrixFr tiled_vertices = tile_vertices(
            get_2D_tiling_operators(m_mesh));
    MatrixIr tiled_edges = tile_edges(num_cells);

    WireNetwork::Ptr tiled_network = WireNetwork::create_raw(tiled_vertices, tiled_edges);
    update_attributes(*tiled_network, num_cells);
    clean_up(*tiled_network);
    return tiled_network;
}

WireNetwork::Ptr MeshTiler::tile_3D() {
    const size_t num_cells = m_mesh->get_num_voxels();
    scale_to_unit_box();
    MatrixFr tiled_vertices = tile_vertices(
            get_3D_tiling_operators(m_mesh));
    MatrixIr tiled_edges = tile_edges(num_cells);

    WireNetwork::Ptr tiled_network = WireNetwork::create_raw(tiled_vertices, tiled_edges);
    update_attributes(*tiled_network, num_cells);
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
