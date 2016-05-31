/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include <string>
#include "SparseSolver.h"

namespace PyMesh {

class SparseSolverFactory {
    public:
        typedef SparseSolver::Ptr SparseSolverPtr;

    public:
        SparseSolverFactory(const std::string& solver_type);
        SparseSolverPtr create() { return m_solver; }

    private:
        SparseSolverPtr m_solver;
};

}
