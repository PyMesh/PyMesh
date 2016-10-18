/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <cassert>
#include <memory>

#include <Core/EigenTypedef.h>
#include <Core/Exception.h>
#include "SparseSolver.h"
#include "third_party/SparseMatrices.hh"

#include <iostream>

namespace PyMesh {

template <typename Engine>
class SparseSolverImplementation : public SparseSolver {
    public:
        virtual void analyze_pattern(const ZSparseMatrix& matrix) {
            m_engine.analyzePattern(matrix);
            check_info();
        }

        virtual void factorize(const ZSparseMatrix& matrix) {
            m_engine.factorize(matrix);
            check_info();
        }

        virtual MatrixF solve(const MatrixF& rhs) {
            auto x = m_engine.solve(rhs);
            return x;
        }

    private:
        void check_info() {
            switch (m_engine.info()) {
                case Eigen::Success:
                    return;
                case Eigen::NumericalIssue:
                    throw RuntimeError("Eigen sparse solver encounters numerical issue");
                    break;
                case Eigen::NoConvergence:
                    throw RuntimeError("Eigen sparse solver did not converge");
                    break;
                case Eigen::InvalidInput:
                    throw RuntimeError("Eigen sparse solver has invalid input");
                    break;
                default:
                    throw RuntimeError("Unknown eigen sparse solver error");
            }
        }

    protected:
        Engine m_engine;
};

//template<>
//class SparseSolverImplementation<UmfpackFactorizer> : public SparseSolver {
//    public:
//        typedef std::shared_ptr<UmfpackFactorizer> FactorizerPtr;
//
//        virtual void analyze_pattern(const ZSparseMatrix& matrix) {}
//
//        virtual void factorize(const ZSparseMatrix& matrix) {
//            m_matrix = ZSparseMatrix2SuiteSparseMatrix(matrix);
//            m_factorizer = FactorizerPtr(new UmfpackFactorizer(m_matrix));
//        }
//
//        virtual MatrixF solve(const MatrixF& rhs) {
//            VectorF sol;
//            m_factorizer->solve(rhs, sol);
//            return sol;
//        }
//
//    protected:
//        SuiteSparseMatrix ZSparseMatrix2SuiteSparseMatrix(
//                const ZSparseMatrix& matrix) const {
//            SuiteSparseMatrix ss_matrix;
//            const size_t outer_size = matrix.get_outer_size();
//            const size_t inner_size = matrix.get_inner_size();
//            const size_t value_size = matrix.get_value_size();
//
//            ss_matrix.Ap.resize(outer_size);
//            ss_matrix.Ai.resize(inner_size);
//            ss_matrix.Ax.resize(value_size);
//
//            std::copy(matrix.outerIndexPtr(), matrix.outerIndexPtr() + outer_size,
//                    ss_matrix.Ap.begin());
//            std::copy(matrix.innerIndexPtr(), matrix.innerIndexPtr() + inner_size,
//                    ss_matrix.Ai.begin());
//            std::copy(matrix.valuePtr(), matrix.valuePtr() + value_size,
//                    ss_matrix.Ax.begin());
//
//            ss_matrix.m = matrix.rows();
//            ss_matrix.n = matrix.cols();
//            ss_matrix.nz = value_size;
//
//            return ss_matrix;
//        }
//
//    protected:
//        SuiteSparseMatrix m_matrix;
//        FactorizerPtr m_factorizer;
//};


}
