#pragma once
#include <tr1/memory>

#include <Core/EigenTypedef.h>

class Mesh;

class Boundary {
    public:
        typedef std::tr1::shared_ptr<Boundary> Ptr;
        static Ptr extract_surface_boundary(const Mesh& mesh);
        static Ptr extract_volume_boundary(const Mesh& mesh);

    public:
        virtual size_t get_num_boundaries() const =0;
        virtual MatrixIr get_boundaries() const =0;
        virtual VectorI get_boundary(size_t bi) const =0;
        virtual size_t get_boundary_element(size_t bi) const =0;
        virtual VectorI get_boundary_nodes() const=0;
};
