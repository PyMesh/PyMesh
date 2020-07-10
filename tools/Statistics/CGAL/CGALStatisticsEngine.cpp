/* This file is part of PyMesh. Copyright (c) 2020 by Julien Jerphanion */
#ifdef WITH_CGAL
#include "CGALStatisticsEngine.h"

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

void CGALStatisticsEngine::convert_mesh_to_native_format() {
    m_mesh = CGALUtils::mesh_to_polyhedron<Kernel>(m_vertices, m_faces);
    m_nef_mesh = Nef_polyhedron(mesh);
}

void CGALStatisticsEngine::compute_statistics() {
    // TODO: adapt
}

#endif
