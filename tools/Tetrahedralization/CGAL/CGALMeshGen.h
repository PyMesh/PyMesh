/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#ifdef WITH_CGAL

#include <Core/Exception.h>
#include <Tetrahedralization/TetrahedralizationEngine.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/make_mesh_3.h>

#include "CGALMeshDomain.h"

#if WITH_IGL
#include "Oracle.h"
#endif

namespace PyMesh {

enum class CGALDomainType {
    EXPLICIT,
    EXPLICIT_WITH_FEATURES,
    IMPLICIT,
    IMPLICIT_WITH_FEATURES
};

using InexactKernel = CGAL::Exact_predicates_inexact_constructions_kernel;
using ExactKernel = CGAL::Exact_predicates_exact_constructions_kernel;

template<typename Kernel, CGALDomainType DomainType>
class CGALMeshGen : public TetrahedralizationEngine {
    public:
        virtual ~CGALMeshGen() = default;

    public:
        virtual void run() override {
            throw NotImplementedError("CGAL domain type must be specified!");
        }

};

template<>
class CGALMeshGen<InexactKernel, CGALDomainType::EXPLICIT_WITH_FEATURES>
: public TetrahedralizationEngine {
    public:
        virtual void run() override {
            using namespace CGAL::parameters;
            using Kernel = InexactKernel;
            preprocess();

            auto domain = create_polyhedral_domain_with_feature<Kernel>(
                    m_vertices, m_faces, m_feature_angle);
            using Domain = typename decltype(domain)::element_type;
            using Traits = CGALMeshTraitsWithFeature<Domain>;
            //dump_features("features.obj", domain);

            typename Traits::Criteria criteria(
                    //edge_size             =m_cell_size * 0.5,
                    cell_radius_edge_ratio=m_cell_radius_edge_ratio,
                    facet_distance        =m_facet_distance,
                    cell_size             =m_cell_size);

            auto c3t3 = CGAL::make_mesh_3<typename Traits::C3t3>(*domain, criteria);
            //dump_edges(c3t3, "edges.txt");
            extract_mesh<Traits>(c3t3, m_vertices, m_faces, m_voxels);
        }
};

template<>
class CGALMeshGen<InexactKernel, CGALDomainType::EXPLICIT>
: public TetrahedralizationEngine {
    public:
        virtual void run() override {
            using namespace CGAL::parameters;
            using Kernel = InexactKernel;
            preprocess();

            auto domain = create_polyhedral_domain<Kernel>(
                    m_vertices, m_faces);
            using Domain = typename decltype(domain)::element_type;
            using Traits = CGALMeshTraits<Domain>;

            //std::vector<std::pair<Domain::Point_3, Domain::Index>> init_pts;
            //auto init_points_constructor =
            //    domain->construct_initial_points_object();
            //init_points_constructor(init_pts.end());
            //std::cout << init_pts.size() << " points generated." << std::endl;

            typename Traits::Criteria criteria(
                    //edge_size             =m_cell_size * 0.5,
                    cell_radius_edge_ratio=m_cell_radius_edge_ratio,
                    facet_distance        =m_facet_distance,
                    cell_size             =m_cell_size);

            auto c3t3 = CGAL::make_mesh_3<typename Traits::C3t3>(*domain, criteria);
            extract_mesh<Traits>(c3t3, m_vertices, m_faces, m_voxels);
        }
};

#if WITH_IGL
template<>
class CGALMeshGen<InexactKernel, CGALDomainType::IMPLICIT_WITH_FEATURES>
: public TetrahedralizationEngine {
    public:
        virtual void run() override {
            using namespace CGAL::parameters;
            using Kernel = InexactKernel;
            using OracleType = Oracle<MatrixFr, MatrixIr>;
            preprocess();

            OracleType oracle(m_vertices, m_faces);

            auto domain = create_implicit_domain_with_features<Kernel, OracleType>(
                    m_vertices, m_faces, oracle);
            using Domain = typename decltype(domain)::element_type;
            using Traits = CGALMeshTraitsWithFeature<Domain>;

            auto_compute_meshing_params();
            typename Traits::Criteria criteria(
                    //edge_size             =m_cell_size * 0.5,
                    cell_radius_edge_ratio=m_cell_radius_edge_ratio,
                    facet_distance        =m_facet_distance,
                    cell_size             =m_cell_size);

            auto c3t3 = CGAL::make_mesh_3<typename Traits::C3t3>(*domain, criteria);
            //std::cout << "Oracle is used " << oracle.get_counter()
            //    << " times in total" << std::endl;
            extract_mesh<Traits>(c3t3, m_vertices, m_faces, m_voxels);
        }
};
#endif


}

#endif
