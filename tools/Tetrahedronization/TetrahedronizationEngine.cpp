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
    if (num_vertices == 0) {
        throw RuntimeError("Input mesh is empty!");
    }
}

void TetrahedronizationEngine::auto_compute_meshing_params() {
    if (m_cell_size < 0.0) {
        const Vector3F bbox_min = m_vertices.colwise().minCoeff();
        const Vector3F bbox_max = m_vertices.colwise().maxCoeff();
        m_cell_size = (bbox_max - bbox_min).norm() / 20;
#ifndef NDEBUG
        std::cout << "using default cell size: " << m_cell_size << std::endl;
#endif
    }
}
