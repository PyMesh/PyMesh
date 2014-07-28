#pragma once

#include <cassert>

#include <Core/EigenTypedef.h>
#include <Core/Exception.h>
#include "SparseSolver.h"

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

        virtual VectorF solve(const VectorF& rhs) {
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
