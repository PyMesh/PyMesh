/* This file is part of PyMesh. Copyright (c) 2017 by Qingnan Zhou */
#include "HarmonicSolver.h"
#include <Core/Exception.h>
#include <Math/MatrixUtils.h>
#include <igl/harmonic.h>
#include <iostream>

using namespace PyMesh;

HarmonicSolver::Ptr HarmonicSolver::create(const Mesh::Ptr mesh) {
    const size_t dim = mesh->get_dim();
    if (dim == 2) {
        const MatrixFr vertices = MatrixUtils::reshape<MatrixFr>(
                mesh->get_vertices(),
                mesh->get_num_vertices(),
                mesh->get_dim());
        const MatrixIr faces = MatrixUtils::reshape<MatrixIr>(
                mesh->get_faces(),
                mesh->get_num_faces(),
                mesh->get_vertex_per_face());
        return std::make_shared<HarmonicSolver>(vertices, faces);
    } else if (dim == 3) {
        const MatrixFr vertices = MatrixUtils::reshape<MatrixFr>(
                mesh->get_vertices(),
                mesh->get_num_vertices(),
                mesh->get_dim());
        const MatrixIr voxels = MatrixUtils::reshape<MatrixIr>(
                mesh->get_voxels(),
                mesh->get_num_voxels(),
                mesh->get_vertex_per_voxel());
        return std::make_shared<HarmonicSolver>(vertices, voxels);
    } else {
        throw NotImplementedError("Unsupported mesh dimension: " + std::to_string(dim));
    }
}

HarmonicSolver::Ptr HarmonicSolver::create_raw(
        const MatrixFr& nodes, const MatrixIr& elements) {
    return std::make_shared<HarmonicSolver>(nodes, elements);
}

void HarmonicSolver::pre_process() {
    const size_t dim = m_nodes.cols();
    const size_t simplex_size = m_elements.cols();
    if (dim == 2) {
        if (simplex_size != 3) {
            throw RuntimeError("In 2D, harmonic solver only supports triangle meshes");
        }
    } else if (dim == 3) {
        if (simplex_size != 4) {
            throw RuntimeError("In 3D, harmonic solver only supports tet meshes");
        }
    } else {
        throw NotImplementedError("Harmonic solver does not support mesh with dimention " +
                std::to_string(dim));
    }
}

void HarmonicSolver::solve() {
    igl::harmonic(m_nodes, m_elements, m_bd_indices, m_bd_values, m_order, m_solution);
}
