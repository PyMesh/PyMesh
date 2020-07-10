/* This file is part of PyMesh. Copyright (c) 2020 by Julien Jerphanion */
#pragma once
#ifdef WITH_CGAL

#include <Statistics/StatisticsEngine.h>
#include <Statistics/MeshStatistics.h>

#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/array.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Nef_polyhedron_2.h>

typedef Kernel::Point_3                                 Point;
typedef CGAL::Surface_mesh<Point>                       Mesh;

namespace PyMesh {

    class CGALStatisticsEngine : public StatisticsEngine {
        public:
            virtual ~CGALStatisticsEngine() = default;

        public:
            virtual MeshStatistics compute_statistics() override;

        protected:
            virtual void convert_mesh_to_native_format() override;

        private:
            Mesh m_mesh
    };

}

#endif
