/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#ifdef WITH_CGAL

#include <Boolean/BooleanEngine.h>

#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Nef_polyhedron_3.h>

namespace PyMesh {

class CGALBooleanEngine : public BooleanEngine {
    public:
        virtual ~CGALBooleanEngine() =default;

    public:
        virtual void compute_union() override;
        virtual void compute_intersection() override;
        virtual void compute_difference() override;
        virtual void compute_symmetric_difference() override;

    protected:
        virtual void convert_mesh_to_native_format(MeshSelection s) override;

    private:
        using Kernel = CGAL::Exact_predicates_exact_constructions_kernel;
        using Nef_polyhedron = CGAL::Nef_polyhedron_3<Kernel>;

        Nef_polyhedron m_nef_mesh_1;
        Nef_polyhedron m_nef_mesh_2;
};

}

#endif
