#pragma once
#include <Core/EigenTypedef.h>

class WindingNumber {
    public:
        virtual void init(const MatrixFr& V, const MatrixIr& F) {
            m_vertices = V;
            m_faces = F;
        }

        virtual VectorF compute(const MatrixFr& P)=0;

    protected:
        MatrixFr m_vertices;
        MatrixIr m_faces;
};
