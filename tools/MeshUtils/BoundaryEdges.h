#pragma once
#include "Boundary.h"

class BoundaryEdges : public Boundary {
    public:
        BoundaryEdges(const Mesh& mesh);

    public:
        virtual size_t get_num_boundaries() const;
        virtual MatrixIr get_boundaries() const;
        virtual VectorI get_boundary(size_t bi) const;
        virtual size_t get_boundary_element(size_t bi) const;

    private:
        void extract_boundary(const Mesh& mesh);

    private:
        MatrixIr m_boundaries;
        VectorI  m_boundary_faces;
};
