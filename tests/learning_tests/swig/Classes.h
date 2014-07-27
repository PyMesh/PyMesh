#pragma once

#include <Core/EigenTypedef.h>
#include <Math/ZSparseMatrix.h>

class SparseMat : public ZSparseMatrix {
    public:
        SparseMat(size_t num_rows, size_t num_cols) : ZSparseMatrix(num_rows, num_cols) {}

        void initialize(VectorI& inner_indices, VectorI& outer_indices, VectorF& values);
};
