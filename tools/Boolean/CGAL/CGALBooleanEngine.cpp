/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "CGALBooleanEngine.h"

#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/Nef_polyhedron_3.h>
#include <CGAL/Polyhedron_incremental_builder_3.h>

#include <Core/EigenTypedef.h>
#include <Core/Exception.h>
#include "CGALUtils.h"

using namespace PyMesh;

typedef CGAL::Exact_predicates_exact_constructions_kernel Kernel;
typedef CGAL::Polyhedron_3<Kernel>  Polyhedron;
typedef CGAL::Nef_polyhedron_3<Kernel> Nef_polyhedron;
typedef Polyhedron::HalfedgeDS HalfedgeDS;

namespace CGALBooleanEngineHelper {
    void assert_mesh_is_closed(const Polyhedron& P) {
        if (!P.is_closed()) {
            throw RuntimeError("Mesh not closed.  Boolean requires closed mesh as input.");
        }
    }
}

using namespace CGALBooleanEngineHelper;

void CGALBooleanEngine::compute_union() {
    Polyhedron mesh1 = CGALUtils::mesh_to_polyhedron<Kernel>(
            m_vertices_1, m_faces_1);
    Polyhedron mesh2 = CGALUtils::mesh_to_polyhedron<Kernel>(
            m_vertices_2, m_faces_2);
    assert_mesh_is_closed(mesh1);
    assert_mesh_is_closed(mesh2);
    Nef_polyhedron nef_mesh1(mesh1);
    Nef_polyhedron nef_mesh2(mesh2);
    Nef_polyhedron nef_result = nef_mesh1 + nef_mesh2;
    if (nef_result.is_simple()) {
        Polyhedron result;
        nef_result.convert_to_polyhedron(result);
        CGALUtils::polyhedron_to_mesh(result, m_vertices, m_faces);
    } else {
        throw RuntimeError("Boolean result is not a simple polyhedron!");
    }
}

void CGALBooleanEngine::compute_intersection() {
    Polyhedron mesh1 = CGALUtils::mesh_to_polyhedron<Kernel>(
            m_vertices_1, m_faces_1);
    Polyhedron mesh2 = CGALUtils::mesh_to_polyhedron<Kernel>(
            m_vertices_2, m_faces_2);
    assert_mesh_is_closed(mesh1);
    assert_mesh_is_closed(mesh2);
    Nef_polyhedron nef_mesh1(mesh1);
    Nef_polyhedron nef_mesh2(mesh2);
    Nef_polyhedron nef_result = nef_mesh1 * nef_mesh2;
    if (nef_result.is_simple()) {
        Polyhedron result;
        nef_result.convert_to_polyhedron(result);
        CGALUtils::polyhedron_to_mesh(result, m_vertices, m_faces);
    } else {
        throw RuntimeError("Boolean result is not a simple polyhedron!");
    }
}

void CGALBooleanEngine::compute_difference() {
    Polyhedron mesh1 = CGALUtils::mesh_to_polyhedron<Kernel>(
            m_vertices_1, m_faces_1);
    Polyhedron mesh2 = CGALUtils::mesh_to_polyhedron<Kernel>(
            m_vertices_2, m_faces_2);
    assert_mesh_is_closed(mesh1);
    assert_mesh_is_closed(mesh2);
    Nef_polyhedron nef_mesh1(mesh1);
    Nef_polyhedron nef_mesh2(mesh2);
    Nef_polyhedron nef_result = nef_mesh1 - nef_mesh2;
    if (nef_result.is_simple()) {
        Polyhedron result;
        nef_result.convert_to_polyhedron(result);
        CGALUtils::polyhedron_to_mesh(result, m_vertices, m_faces);
    } else {
        throw RuntimeError("Boolean result is not a simple polyhedron!");
    }
}

void CGALBooleanEngine::compute_symmetric_difference() {
    Polyhedron mesh1 = CGALUtils::mesh_to_polyhedron<Kernel>(
            m_vertices_1, m_faces_1);
    Polyhedron mesh2 = CGALUtils::mesh_to_polyhedron<Kernel>(
            m_vertices_2, m_faces_2);
    assert_mesh_is_closed(mesh1);
    assert_mesh_is_closed(mesh2);
    Nef_polyhedron nef_mesh1(mesh1);
    Nef_polyhedron nef_mesh2(mesh2);
    Nef_polyhedron nef_result = nef_mesh1 ^ nef_mesh2;
    if (nef_result.is_simple()) {
        Polyhedron result;
        nef_result.convert_to_polyhedron(result);
        CGALUtils::polyhedron_to_mesh(result, m_vertices, m_faces);
    } else {
        throw RuntimeError("Boolean result is not a simple polyhedron!");
    }
}

