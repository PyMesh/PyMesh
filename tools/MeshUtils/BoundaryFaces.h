/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include "Boundary.h"
#include <Core/EigenTypedef.h>

namespace PyMesh {

class BoundaryFaces : public Boundary {
    public:
        BoundaryFaces(const Mesh& mesh);

    public:
        virtual size_t get_num_boundaries() const;
        virtual MatrixIr get_boundaries() const;
        virtual VectorI get_boundary(size_t bi) const;
        virtual size_t get_boundary_element(size_t bi) const;

        virtual size_t get_num_boundary_nodes() const;
        virtual VectorI get_boundary_nodes() const;

    private:
        void extract_boundary(const Mesh& mesh);
        void extract_boundary_nodes();

    private:
        MatrixIr m_boundaries;
        VectorI m_boundary_voxels;
        VectorI m_boundary_nodes;
};

}
