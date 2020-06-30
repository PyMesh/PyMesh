/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "TetrahedralizationEngine.h"

#include <cmath>
#include <sstream>
#include <iostream>

#include <Core/Exception.h>

#if WITH_CGAL
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include "CGAL/CGALMeshGen.h"
#endif

#if WITH_TETGEN
#include "TetGen/TetGenEngine.h"
#endif

#if WITH_GEOGRAM
#include "Geogram/GeogramEngine.h"
#endif

#if WITH_QUARTET
#include "Quartet/QuartetEngine.h"
#endif

#if WITH_MMG && WITH_TETGEN && WITH_IGL
#include "MMG/MMGEngine.h"
#endif

#if WITH_TETWILD
#include "TetWild/TetWildEngine.h"
#endif

using namespace PyMesh;

TetrahedralizationEngine::Ptr TetrahedralizationEngine::create(
        const std::string& engine_name) {
#if WITH_CGAL
    using InexactKernel = CGAL::Exact_predicates_inexact_constructions_kernel;
    using ExactKernel = CGAL::Exact_predicates_exact_constructions_kernel;
    if (engine_name == "cgal") {
        using CGALEngine = CGALMeshGen<
            InexactKernel,
            CGALDomainType::EXPLICIT_WITH_FEATURES>;
        return std::make_shared<CGALEngine>();
    }
    if (engine_name == "cgal_no_features") {
        using CGALEngine = CGALMeshGen<
            InexactKernel,
            CGALDomainType::EXPLICIT>;
        return std::make_shared<CGALEngine>();
    }
#if WITH_IGL
    if (engine_name == "cgal_implicit") {
        using CGALEngine = CGALMeshGen<
            InexactKernel,
            CGALDomainType::IMPLICIT_WITH_FEATURES>;
        return std::make_shared<CGALEngine>();
    }
#endif
#endif
#if WITH_TETGEN
    if (engine_name == "tetgen") { return Ptr(new TetGenEngine); }
#if WITH_MMG && WITH_IGL
    if (engine_name == "mmg") { return Ptr(new MMGEngine); }
#endif
#endif
#if WITH_GEOGRAM
    if (engine_name == "geogram") { return Ptr(new GeogramEngine); }
#endif
#if WITH_QUARTET
    if (engine_name == "quartet") { return Ptr(new QuartetEngine); }
#endif
#if WITH_TETWILD
    if (engine_name == "tetwild") { return Ptr(new TetWildEngine); }
#endif

    std::stringstream err_msg;
    err_msg << "Tetrahedralization engine " << engine_name
        << " is not supported.";
    throw NotImplementedError(err_msg.str());
}

void TetrahedralizationEngine::preprocess() {
    assert_mesh_is_valid();
    auto_compute_meshing_params();
}

void TetrahedralizationEngine::assert_mesh_is_valid() const {
    const size_t num_vertices = m_vertices.rows();
    const size_t dim = m_vertices.cols();
    const size_t vertex_per_face = m_faces.cols();

    if (dim != 3) {
        throw NotImplementedError(
                "Tetrahedralization Only support 3D mesh");
    }
    if (vertex_per_face != 3) {
        throw NotImplementedError(
                "Tetrahedralization only supports triangular mesh");
    }
    if (num_vertices == 0) {
        throw RuntimeError("Input mesh is empty!");
    }
}

void TetrahedralizationEngine::auto_compute_meshing_params() {
    if (m_cell_size < 0.0) {
        const Vector3F bbox_min = m_vertices.colwise().minCoeff();
        const Vector3F bbox_max = m_vertices.colwise().maxCoeff();
        m_cell_size = (bbox_max - bbox_min).norm() / 20;
#ifndef NDEBUG
        std::cout << "using default cell size: " << m_cell_size << std::endl;
#endif
    }
}
