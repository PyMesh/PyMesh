/* This file is part of PyMesh. Copyright (c) 2018 by Qingnan Zhou */
#ifdef WITH_TETWILD
#include "TetWildEngine.h"

#include <array>
#include <cassert>
#include <vector>

#include <tetwild.h>

using namespace PyMesh;

void TetWildEngine::run() {
    using Points = std::vector<std::array<double, 3>>;
    using Triangles = std::vector<std::array<int, 3>>;
    using Tets = std::vector<std::array<int, 4>>;

    preprocess();
    const size_t num_vertices = m_vertices.rows();
    const size_t num_faces = m_faces.rows();
    Points V_in(num_vertices);
    for (size_t i=0; i<num_vertices; i++) {
        V_in[i] = {m_vertices(i,0), m_vertices(i,1), m_vertices(i,2)};
    }
    Triangles F_in(num_faces);
    for (size_t i=0; i<num_faces; i++) {
        F_in[i] = {m_faces(i,0), m_faces(i,1), m_faces(i,2)};
    }

    const Vector3F bbox_min = m_vertices.colwise().minCoeff();
    const Vector3F bbox_max = m_vertices.colwise().maxCoeff();
    const Float diag = (bbox_max - bbox_min).norm();

    tetwild::parameters.i_ideal_edge_length = diag /
        (4.0 / sqrt(6.0) * m_cell_size);
    assert(tetwild::parameters.i_ideal_edge_length > 0.0);
    if (m_facet_distance > 0) {
        tetwild::parameters.i_epsilon = diag / m_facet_distance;
        assert(tetwild::parameters.i_epsilon >= 0.0);
    }
    tetwild::parameters.is_quiet = true;

    Points V_out;
    Tets T_out;
    tetwild::tetrahedralization(V_in, F_in, V_out, T_out);

    const size_t num_out_vertices = V_out.size();
    const size_t num_out_tets = T_out.size();
    m_vertices.resize(num_out_vertices, 3);
    for (size_t i=0; i<num_out_vertices; i++) {
        const auto& v = V_out[i];
        m_vertices.row(i) << v[0], v[1], v[2];
    }
    m_voxels.resize(num_out_tets, 4);
    for (size_t i=0; i<num_out_tets; i++) {
        const auto& t = T_out[i];
        m_voxels.row(i) << t[0], t[1], t[2], t[3];
    }
    m_faces.resize(0, 3);
}

#endif
