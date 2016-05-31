/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include <memory>

#include <Core/EigenTypedef.h>

namespace PyMesh {

class Mesh;

class Boundary {
    public:
        typedef std::shared_ptr<Boundary> Ptr;
        static Ptr extract_surface_boundary(const Mesh& mesh);
        static Ptr extract_surface_boundary_raw(MatrixFr& vertices, MatrixIr& faces);
        static Ptr extract_volume_boundary(const Mesh& mesh);
        static Ptr extract_volume_boundary_raw(MatrixFr& vertices, MatrixIr& voxels);

    public:
        virtual size_t get_num_boundaries() const =0;
        virtual MatrixIr get_boundaries() const =0;
        virtual VectorI get_boundary(size_t bi) const =0;
        virtual size_t get_boundary_element(size_t bi) const =0;

        virtual size_t get_num_boundary_nodes() const=0;
        virtual VectorI get_boundary_nodes() const=0;
};

}
