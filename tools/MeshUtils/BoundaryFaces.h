#pragma once
#include "Boundary.h"
#include <Core/EigenTypedef.h>

class BoundaryFaces : public Boundary {
    public:
        BoundaryFaces(const Mesh& mesh);

    public:
        virtual size_t get_num_boundaries() const;
        virtual MatrixIr get_boundaries() const;
        virtual VectorI get_boundary(size_t bi) const;

    private:
        void extract_boundary(const Mesh& mesh);

    private:
        MatrixIr m_boundaries;
};
