/* This file is part of PyMesh. Copyright (c) 2018 by Qingnan Zhou */
#ifdef WITH_TETWILD
#include "TetWildEngine.h"

#include <array>
#include <cassert>
#include <vector>

#include <tetwild/tetwild.h>

using namespace PyMesh;

void TetWildEngine::run() {
    preprocess();
    tetwild::Args args;

    const Vector3F bbox_min = m_vertices.colwise().minCoeff();
    const Vector3F bbox_max = m_vertices.colwise().maxCoeff();
    const Float diag = (bbox_max - bbox_min).norm();

    args.initial_edge_len_rel = (4.0 / sqrt(6.0) * m_cell_size) / diag;
    if (m_facet_distance > 0) {
        args.eps_rel = m_facet_distance / diag;
    }
    args.is_quiet = true;

    MatrixF in_vertices, out_vertices;
    MatrixI in_faces, out_voxels;

    const size_t num_vertices = m_vertices.rows();
    const size_t num_faces = m_faces.rows();
    in_vertices.resize(num_vertices, 3);
    in_faces.resize(num_faces, 3);

    for (size_t i=0; i<num_vertices; i++) {
        in_vertices.row(i) = m_vertices.row(i);
    }
    for (size_t i=0; i<num_faces; i++) {
        in_faces.row(i) = m_faces.row(i);
    }

    VectorF dihedral_angles;
    tetwild::tetrahedralization(in_vertices, in_faces,
            out_vertices, out_voxels, dihedral_angles, args);

    const size_t num_out_vertices = out_vertices.rows();
    const size_t num_out_voxels = out_voxels.rows();
    m_vertices.resize(num_out_vertices, 3);
    m_voxels.resize(num_out_voxels, 4);

    for (size_t i=0; i<num_out_vertices; i++) {
        m_vertices.row(i) = out_vertices.row(i);
    }
    for (size_t i=0; i<num_out_voxels; i++) {
        m_voxels.row(i) = out_voxels.row(i);
    }
    m_faces.resize(0, 3);
}

#endif
