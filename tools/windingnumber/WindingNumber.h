#pragma once
#include "EigenTypedef.h"
#include "AABB.h"

class WindingNumber {
    public:
        WindingNumber(const MatrixF& V, const MatrixI& F);

    public:
        VectorF compute(const MatrixF& P);

        //static VectorF compute(const MatrixF& V, const MatrixI& F, const MatrixF& P);
    private:
        AABB<Vector3F> m_hierarchy;
};
