#pragma once
#include <memory>
#include <Core/EigenTypedef.h>
#include <Math/ZSparseMatrix.h>

class SparseSolver {
    public:
        typedef std::shared_ptr<SparseSolver> Ptr;
        virtual ~SparseSolver() {}

    public:
        virtual void analyze_pattern(const ZSparseMatrix& matrix) =0;

        virtual void factorize(const ZSparseMatrix& matrix) =0;

        virtual VectorF solve(const VectorF& rhs) =0;
};
