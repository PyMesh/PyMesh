#pragma once
#include "EigenTypedef.h"

class WindingNumber {
    public:
        static VectorF compute(const MatrixF& V, const MatrixI& F, const MatrixF& P);
};
