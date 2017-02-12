/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "TetrahedronizationEngine.h"

#include <cmath>
#include <sstream>
#include <iostream>

#include <Core/Exception.h>

#if WITH_CGAL
#include "CGAL/CGALMeshGen.h"
#endif

#if WITH_TETGEN
#include "TetGen/TetGenEngine.h"
#endif

#if WITH_GEOGRAM
#include "GeoGram/GeoGramEngine.h"
#endif

#if WITH_QUARTET
#include "Quartet/QuartetEngine.h"
#endif

using namespace PyMesh;

TetrahedronizationEngine::Ptr TetrahedronizationEngine::create(
        const std::string& engine_name) {
#if WITH_CGAL
    if (engine_name == "cgal") { return Ptr(new CGALMeshGen); }
#endif
#if WITH_TETGEN
    if (engine_name == "tetgen") { return Ptr(new TetGenEngine); }
#endif
#if WITH_GEOGRAM
    if (engine_name == "geogram") { return Ptr(new GeoGramEngine); }
#endif
#if WITH_QUARTET
    if (engine_name == "quartet") { return Ptr(new QuartetEngine); }
#endif

    std::stringstream err_msg;
    err_msg << "Tetrahedronization engine " << engine_name
        << " is not supported.";
    throw NotImplementedError(err_msg.str());
}

void TetrahedronizationEngine::preprocess() {
    assert_mesh_is_valid();
    compute_ave_edge_length();
    auto_compute_meshing_params();
}

void TetrahedronizationEngine::assert_mesh_is_valid() const {
    const size_t num_vertices = m_vertices.rows();
    const size_t num_faces = m_faces.rows();
    const size_t dim = m_vertices.cols();
    const size_t vertex_per_face = m_faces.cols();

    if (dim != 3) {
        throw NotImplementedError(
                "Tetrahedronization Only support 3D mesh");
    }
    if (vertex_per_face != 3) {
        throw NotImplementedError(
                "Tetrahedronization only supports triangular mesh");
    }
    if (num_vertices == 0 || num_faces == 0) {
        throw RuntimeError("Input mesh is empty!");
    }
}

void TetrahedronizationEngine::compute_ave_edge_length() {
    Float total_edge_len = 0;

    const size_t num_faces = m_faces.rows();
    for (size_t i=0; i<num_faces; i++) {
        const auto& face = m_faces.row(i);
        const auto& v0 = m_vertices.row(face[0]);
        const auto& v1 = m_vertices.row(face[1]);
        const auto& v2 = m_vertices.row(face[2]);
        total_edge_len += (v1 - v0).norm();
        total_edge_len += (v2 - v1).norm();
        total_edge_len += (v0 - v2).norm();
    }

    const size_t num_edges = num_faces * 3;
    m_ave_edge_length = total_edge_len / num_edges;
}

void TetrahedronizationEngine::auto_compute_meshing_params() {
    if (m_edge_size < 0.0) {
       m_edge_size = m_ave_edge_length * 2;
#ifndef NDEBUG
       std::cout << "using default edge size: " << m_edge_size << std::endl;
#endif
    }
    if (m_face_size < 0.0) {
        m_face_size = m_ave_edge_length;
#ifndef NDEBUG
        std::cout << "using default face size: " << m_face_size << std::endl;
#endif
    }
    if (m_cell_size < 0.0) {
        m_cell_size = m_ave_edge_length;
#ifndef NDEBUG
        std::cout << "using default cell size: " << m_cell_size << std::endl;
#endif
    }
}
