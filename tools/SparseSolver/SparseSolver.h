/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include <memory>
#include <Core/EigenTypedef.h>
#include <Core/Exception.h>
#include <Math/ZSparseMatrix.h>

namespace PyMesh {

class SparseSolver {
    public:
        typedef std::shared_ptr<SparseSolver> Ptr;
        static Ptr create(const std::string& solve_type);

    public:
        virtual void analyze_pattern(const ZSparseMatrix& matrix) {
            throw NotImplementedError(
                    "SparseSolver::analyze_pattern is not implemented");
        }

        virtual void factorize(const ZSparseMatrix& matrix) {
            throw NotImplementedError(
                    "SparseSolver::facetorize is not implemented");
        }

        virtual MatrixF solve(const MatrixF& rhs) {
            throw NotImplementedError(
                    "SparseSolver::facetorize is not implemented");
        }
};

}
