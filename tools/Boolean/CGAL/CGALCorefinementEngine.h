/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#ifdef WITH_CGAL
#ifdef WITH_CGAL_COREFINEMENT

#include <Boolean/BooleanEngine.h>

#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Surface_mesh.h>

namespace PyMesh {

class CGALCorefinementEngine : public BooleanEngine {
    public:
        virtual ~CGALCorefinementEngine() =default;

    public:
        virtual void compute_union() override;
        virtual void compute_intersection() override;
        virtual void compute_difference() override;
        virtual void compute_symmetric_difference() override;

    private:
        using Kernel = CGAL::Exact_predicates_exact_constructions_kernel;
        using SurfaceMesh = CGAL::Surface_mesh<Kernel::Point_3>;

        virtual void convert_mesh_to_native_format(MeshSelection s) override;

        SurfaceMesh m_surface_mesh_1;
        SurfaceMesh m_surface_mesh_2;
};

}

#endif
#endif
