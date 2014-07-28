#include "SparseSolverFactory.h"
#include <sstream>

#include <Eigen/SparseCholesky>
#include <Eigen/IterativeLinearSolvers>
#include <Eigen/SparseLU>

#include <Core/Exception.h>

#include "SparseSolverImplementation.h"

SparseSolverFactory::SparseSolverFactory(const std::string& solver_type) {
    if (solver_type == "LDLT") {
        m_solver = SparseSolverPtr(
                new SparseSolverImplementation<Eigen::SimplicialLDLT<ZSparseMatrix> >);
    } else if (solver_type == "LLT") {
        m_solver = SparseSolverPtr(
                new SparseSolverImplementation<Eigen::SimplicialLLT<ZSparseMatrix> >);
    } else if (solver_type == "CG") {
        m_solver = SparseSolverPtr(
                new SparseSolverImplementation<Eigen::ConjugateGradient<ZSparseMatrix> >);
    } else if (solver_type == "SparseLU") {
        m_solver = SparseSolverPtr(
                new SparseSolverImplementation<Eigen::SparseLU<ZSparseMatrix> >);
    } else {
        std::stringstream err_msg;
        err_msg << "Unsupported solver type " << solver_type;
        throw NotImplementedError(err_msg.str());
    }
}
