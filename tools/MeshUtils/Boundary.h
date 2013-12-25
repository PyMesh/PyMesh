#pragma once
#include <tr1/memory>

#include <Core/EigenTypedef.h>

class Mesh;

class Boundary {
    public:
        typedef std::tr1::shared_ptr<Boundary> BoundaryPtr;
        static BoundaryPtr extract_boundary(const Mesh& mesh);

    public:
        virtual size_t get_num_boundaries() const =0;
        virtual MatrixIr get_boundaries() const =0;
        virtual VectorI get_boundary(size_t bi) const =0;
};
