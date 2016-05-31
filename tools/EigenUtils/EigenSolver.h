/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include <Core/EigenTypedef.h>
#include <Eigen/Eigenvalues>

namespace PyMesh {

/**
 * A thin wrapper around Eigen::EigenSolver<MatrixF>
 */
class EigenSolver {
    public:
        EigenSolver() {}

        void compute(const MatrixF& matrix);

        void compute_batch_symmetric(size_t dim, VectorF matrices);

        VectorF get_eigen_values() const { return m_eigen_values; }
        MatrixF get_eigen_vectors() const { return m_eigen_vectors; }

    private:
        void compute_batch_symmetric_2x2(const VectorF& matrices);
        void compute_batch_symmetric_3x3(const VectorF& matrices);

    private:
        typedef Eigen::EigenSolver<MatrixF> Solver;
        //typedef Solver::EigenvalueType EigenvalueType;
        //typedef Solver::EigenvectorsType EigenvectorsType;

        Solver m_solver;
        VectorF m_eigen_values;
        MatrixF m_eigen_vectors;
};

}
