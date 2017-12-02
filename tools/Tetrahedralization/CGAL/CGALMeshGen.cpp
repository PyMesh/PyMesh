///* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
//#include "CGALMeshGen.h"
//
//#include <cassert>
//#include <iostream>
//#include <fstream>
//
//#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
//#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
//#include <CGAL/make_mesh_3.h>
//#include "CGALMeshDomain.h"
//
//using namespace PyMesh;
//
//typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
////typedef CGAL::Exact_predicates_exact_constructions_kernel K;
//using namespace CGAL::parameters;
//
//void CGALMeshGen::run() {
//    preprocess();
//
//    auto domain = create_polyhedral_domain_with_feature<K>(m_vertices, m_faces);
//    using Domain = decltype(domain)::element_type;
//    using Traits = CGALMeshTraits<Domain>;
//
//    auto_compute_meshing_params();
//    Traits::Criteria criteria(
//            cell_radius_edge_ratio=m_cell_radius_edge_ratio,
//            cell_size             =m_cell_size);
//
//    auto c3t3 = CGAL::make_mesh_3<Traits::C3t3>(*domain, criteria);
//    extract_mesh<Traits>(c3t3, m_vertices, m_faces, m_voxels);
//}
//
