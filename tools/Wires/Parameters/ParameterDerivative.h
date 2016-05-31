/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <memory>
#include <cassert>

#include <Mesh.h>
#include <Core/Exception.h>
#include "PatternParameter.h"

namespace PyMesh {

class ParameterDerivative {
    public:
        typedef std::shared_ptr<ParameterDerivative> Ptr;

    public:
        ParameterDerivative(Mesh::Ptr mesh, PatternParameter::Ptr param);
        virtual ~ParameterDerivative() {}

        /**
         * The method compute the rate of change of the mesh with respect to
         * change in the parameter.  The output is a "#v x dim" matrix where #v
         * is the numver of vertices, and dim is the dimention of the embedding
         * space.
         */
        virtual MatrixFr compute()=0;

    protected:
        void initialize_wires();
        void initialize_mesh();
        void initialize_normals();
        void initialize_face_voronoi_areas();

    protected:
        Mesh::Ptr m_mesh;
        PatternParameter::Ptr m_parameter;
        VectorI m_face_source;
        MatrixFr m_face_normals;
        MatrixFr m_face_voronoi_areas;
};

}
