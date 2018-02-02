/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include <memory>
#include <string>
#include <vector>

#include <Core/EigenTypedef.h>
#include <Core/Exception.h>
#include <Math/ZSparseMatrix.h>

namespace PyMesh {

class SparseSolver {
    public:
        typedef std::shared_ptr<SparseSolver> Ptr;
        static Ptr create(const std::string& solve_type);
        static std::vector<std::string> get_supported_solvers();

    public:
        void set_tolerance(const Float tol) {
            m_tol = tol;
        }

        Float get_tolerance() const {
            return m_tol;
        }

        void set_max_iterations(int max_itr) {
            m_max_iterations = max_itr;
        }

        int get_max_iterations() const {
            return m_max_iterations;
        }

    public:
        virtual void compute(const ZSparseMatrix& matrix) {
            throw NotImplementedError(
                    "SparseSolver::compute is not implemented");
        }

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

        ZSparseMatrix test(const ZSparseMatrix& matrix) {
            return matrix;
        }

    protected:
        Float m_tol = Eigen::NumTraits<Float>::epsilon();
        int m_max_iterations = -1;
};

}
