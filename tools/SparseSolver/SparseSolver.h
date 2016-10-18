/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include <memory>
#include <Core/EigenTypedef.h>
#include <Math/ZSparseMatrix.h>

namespace PyMesh {

class SparseSolver {
    public:
        typedef std::shared_ptr<SparseSolver> Ptr;
        virtual ~SparseSolver() {}

    public:
        virtual void analyze_pattern(const ZSparseMatrix& matrix) =0;

        virtual void factorize(const ZSparseMatrix& matrix) =0;

        virtual MatrixF solve(const MatrixF& rhs) =0;
};

}
