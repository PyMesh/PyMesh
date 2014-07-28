#pragma once

#include <cassert>

#include <Core/EigenTypedef.h>
#include "SparseSolver.h"

template <typename Engine>
class SparseSolverImplementation : public SparseSolver {
    public:
        virtual void analyze_pattern(const ZSparseMatrix& matrix) {
            m_engine.analyzePattern(matrix);
            assert(m_engine.info() == Eigen::Success);
        }

        virtual void factorize(const ZSparseMatrix& matrix) {
            m_engine.factorize(matrix);
            assert(m_engine.info() == Eigen::Success);
        }

        virtual VectorF solve(const VectorF& rhs) {
            auto x = m_engine.solve(rhs);
            return x;
        }

    protected:
        Engine m_engine;
};
