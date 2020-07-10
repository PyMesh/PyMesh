/* This file is part of PyMesh. Copyright (c) 2020 by Julien Jerphanion */
#pragma once
#ifdef WITH_CGAL

#include <Statistics/StatisticsEngine.h>

#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/Nef_polyhedron_3.h>

namespace PyMesh {

class CGALStatisticsEngine : public StatisticsEngine {
    public:
        virtual ~CGALStatisticsEngine() = default;

    public:
        virtual void compute_statistics() override;

    protected:
        virtual void convert_mesh_to_native_format() override;

    private:
        using Kernel = CGAL::Exact_predicates_exact_constructions_kernel;
        using Nef_polyhedron = CGAL::Nef_polyhedron_3<Kernel>;

        Mesh m_mesh
        Nef_polyhedron m_nef_mesh;
};

}

#endif
