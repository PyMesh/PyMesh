/* This file is part of PyMesh. Copyright (c) 2019 by Qingnan Zhou */
#ifdef WITH_IGL

#include "DiskCutter.h"
#include <igl/cut_to_disk.h>
#include <vector>

#include <Math/MatrixUtils.h>
#include <MeshFactory.h>
#include <MeshUtils/MeshCutter.h>

using namespace PyMesh;

void DiskCutter::run() {
    std::vector<std::vector<int>> cuts;
    igl::cut_to_disk(m_faces, cuts);

    MatrixIr voxels(0, 4);
    Mesh::Ptr mesh = MeshFactory().load_matrices(m_vertices, m_faces, voxels).create();
    MeshCutter cutter(mesh);
    Mesh::Ptr cut_mesh = cutter.cut_along_edges(cuts);

    m_out_vertices = MatrixUtils::reshape<MatrixFr>(cut_mesh->get_vertices(),
            cut_mesh->get_num_vertices(), cut_mesh->get_dim());
    m_out_faces = MatrixUtils::reshape<MatrixIr>(cut_mesh->get_faces(),
            cut_mesh->get_num_faces(), cut_mesh->get_vertex_per_face());
}

#endif
