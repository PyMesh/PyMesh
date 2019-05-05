/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#ifdef WITH_CORK

#include <Boolean/BooleanEngine.h>
#include <cork.h>

namespace PyMesh {

class CorkEngine : public BooleanEngine {
    public:
        virtual ~CorkEngine();

    public:
        virtual void compute_union();
        virtual void compute_intersection();
        virtual void compute_difference();
        virtual void compute_symmetric_difference();

    protected:
        virtual void convert_mesh_to_native_format(MeshSelection s);

    private:
        CorkTriMesh m_mesh_1;
        CorkTriMesh m_mesh_2;
};

}

#endif
