/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "CGALCorefinementEngine.h"
#include "CGALUtils.h"

#include <vector>

#include <CGAL/corefinement_operations.h>
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Polyhedron_3.h>

#include <Core/Exception.h>

using namespace PyMesh;

namespace CGALCorefinementEngineHelper {
    typedef CGAL::Exact_predicates_exact_constructions_kernel Kernel;
    typedef CGAL::Polyhedron_3<Kernel>  Polyhedron;
    typedef CGAL::Polyhedron_corefinement<Polyhedron> Corefinement;
}
using namespace CGALCorefinementEngineHelper;

void CGALCorefinementEngine::compute_union() {
    Polyhedron mesh1 = CGALUtils::mesh_to_polyhedron<Kernel>(
            m_vertices_1, m_faces_1);
    Polyhedron mesh2 = CGALUtils::mesh_to_polyhedron<Kernel>(
            m_vertices_2, m_faces_2);

    std::vector<std::pair <Polyhedron*,int> > result;                              
    result.reserve(1);  
    Corefinement coref;
    CGAL::Emptyset_iterator polyline_output;

    coref(mesh1, mesh2, polyline_output,
            std::back_inserter(result),
            Corefinement::Join_tag);

    if (result.size() == 1) {
        CGALUtils::polyhedron_to_mesh(*result[0].first, m_vertices, m_faces);
        delete result[0].first;
    } else if (result.size() == 0) {
        // empty mesh.
    } else {
        throw RuntimeError("More than one polyhedron is returned");
    }
}

void CGALCorefinementEngine::compute_intersection() {
    Polyhedron mesh1 = CGALUtils::mesh_to_polyhedron<Kernel>(
            m_vertices_1, m_faces_1);
    Polyhedron mesh2 = CGALUtils::mesh_to_polyhedron<Kernel>(
            m_vertices_2, m_faces_2);

    std::vector<std::pair <Polyhedron*,int> > result;                              
    result.reserve(1);  
    Corefinement coref;
    CGAL::Emptyset_iterator polyline_output;

    coref(mesh1, mesh2, polyline_output,
            std::back_inserter(result),
            Corefinement::Intersection_tag);

    if (result.size() == 1) {
        CGALUtils::polyhedron_to_mesh(*result[0].first, m_vertices, m_faces);
        delete result[0].first;
    } else if (result.size() == 0) {
        // empty mesh.
    } else {
        throw RuntimeError("More than one polyhedron is returned");
    }
}

void CGALCorefinementEngine::compute_difference() {
    Polyhedron mesh1 = CGALUtils::mesh_to_polyhedron<Kernel>(
            m_vertices_1, m_faces_1);
    Polyhedron mesh2 = CGALUtils::mesh_to_polyhedron<Kernel>(
            m_vertices_2, m_faces_2);

    std::vector<std::pair <Polyhedron*,int> > result;                              
    result.reserve(1);  
    Corefinement coref;
    CGAL::Emptyset_iterator polyline_output;

    coref(mesh1, mesh2, polyline_output,
            std::back_inserter(result),
            Corefinement::P_minus_Q_tag);

    if (result.size() == 1) {
        CGALUtils::polyhedron_to_mesh(*result[0].first, m_vertices, m_faces);
        delete result[0].first;
    } else if (result.size() == 0) {
        // empty mesh.
    } else {
        throw RuntimeError("More than one polyhedron is returned");
    }
}

void CGALCorefinementEngine::compute_symmetric_difference() {
    Polyhedron mesh1 = CGALUtils::mesh_to_polyhedron<Kernel>(
            m_vertices_1, m_faces_1);
    Polyhedron mesh2 = CGALUtils::mesh_to_polyhedron<Kernel>(
            m_vertices_2, m_faces_2);
    throw NotImplementedError("Not implemented");
}
