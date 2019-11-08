/* This file is part of PyMesh. Copyright (c) 2017 by Qingnan Zhou */
#ifdef WITH_IGL
#include "HarmonicSolver.h"
#include <Core/Exception.h>
#include <Math/MatrixUtils.h>
#include <igl/harmonic.h>
#include <iostream>

using namespace PyMesh;

HarmonicSolver::Ptr HarmonicSolver::create(const Mesh::Ptr mesh) {
    const bool is_volume_mesh = (mesh->get_dim() == 3) && (mesh->get_num_voxels() > 0);
    HarmonicSolver::Ptr solver;
    const MatrixFr vertices = MatrixUtils::reshape<MatrixFr>(
            mesh->get_vertices(),
            mesh->get_num_vertices(),
            mesh->get_dim());
    
    if (is_volume_mesh) {
        const MatrixIr voxels = MatrixUtils::reshape<MatrixIr>(
                mesh->get_voxels(),
                mesh->get_num_voxels(),
                mesh->get_vertex_per_voxel());
        solver = std::make_shared<HarmonicSolver>(vertices, voxels);
    } else {
        const MatrixIr faces = MatrixUtils::reshape<MatrixIr>(
                mesh->get_faces(),
                mesh->get_num_faces(),
                mesh->get_vertex_per_face());
        solver = std::make_shared<HarmonicSolver>(vertices, faces);
    } 
    solver->pre_process(); // Check dimensions
    return solver;
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
        if ((simplex_size != 4) && (simplex_size != 3)) {
            throw RuntimeError("In 3D, harmonic solver only supports tet meshes (volume) or triangle meshes (surface).");
        }
    } else {
        throw NotImplementedError("Harmonic solver does not support mesh with dimension " +
                std::to_string(dim));
    }
}

void HarmonicSolver::solve() {
    igl::harmonic(m_nodes, m_elements, m_bd_indices, m_bd_values, m_order, m_solution);
}

#endif
