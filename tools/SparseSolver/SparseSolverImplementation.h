/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <cassert>
#include <memory>

#include <Core/EigenTypedef.h>
#include <Core/Exception.h>
#include "SparseSolver.h"
//#include "third_party/SparseMatrices.hh"

#include <iostream>

namespace PyMesh {

template <typename Engine>
class SparseSolverImplementation : public SparseSolver {
    public:
        virtual void analyze_pattern(const ZSparseMatrix& matrix) {
            m_engine.analyzePattern(matrix);
            // Some engines such as SparseLU left info as undefined at this
            // stage...  So skipping the check for now.
            //check_info("analyze_pattern");
        }

        virtual void factorize(const ZSparseMatrix& matrix) {
            m_engine.factorize(matrix);
            check_info("factorize");
        }

        virtual MatrixF solve(const MatrixF& rhs) {
            auto x = m_engine.solve(rhs);
            check_info("solve");
            return x;
        }

    private:
        void check_info(const std::string& stage="") {
            switch (m_engine.info()) {
                case Eigen::Success:
                    return;
                case Eigen::NumericalIssue:
                    throw RuntimeError(stage +
                            ": Eigen sparse solver encounters numerical issue");
                    break;
                case Eigen::NoConvergence:
                    throw RuntimeError(stage +
                            ": Eigen sparse solver did not converge");
                    break;
                case Eigen::InvalidInput:
                    throw RuntimeError(stage +
                            ": Eigen sparse solver has invalid input");
                    break;
                default:
                    throw RuntimeError(stage +
                            ": Unknown eigen sparse solver error (" +
                            std::to_string(m_engine.info()) + ")");
            }
        }

    protected:
        Engine m_engine;
};

}
