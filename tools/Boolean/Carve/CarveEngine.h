/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#ifdef WITH_CARVE

#include <Boolean/BooleanEngine.h>
#include <carve/csg.hpp>

namespace PyMesh {

class CarveEngine : public BooleanEngine {
    public:
        virtual ~CarveEngine() {}

    public:
        virtual void compute_union();
        virtual void compute_intersection();
        virtual void compute_difference();
        virtual void compute_symmetric_difference();

    protected:
        virtual void convert_mesh_to_native_format(MeshSelection s);

    private:
        using CarveMesh = carve::csg::CSG::meshset_t;
        using CarveMeshPtr = std::shared_ptr<CarveMesh>;
        CarveMeshPtr m_mesh_1;
        CarveMeshPtr m_mesh_2;
};

}

#endif
