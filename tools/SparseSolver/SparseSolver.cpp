/* This file is part of PyMesh. Copyright (c) 2018 by Qingnan Zhou */
#include "SparseSolver.h"
#include <sstream>

#include <Eigen/IterativeLinearSolvers>
#include <Eigen/SparseCholesky>
#include <Eigen/SparseLU>
#include <Eigen/SparseQR>
#ifdef WITH_UMFPACK
#include <Eigen/UmfPackSupport>
#endif
#ifdef WITH_CHOLMOD
#include <Eigen/CholmodSupport>
#endif
//#include <Eigen/PardisoSupport>
//#include <third_party/SparseMatrices.hh>

#include <Core/Exception.h>

#include "SparseSolverImplementation.h"

using namespace PyMesh;

SparseSolver::Ptr SparseSolver::create(const std::string& solver_type) {
    if (solver_type == "LDLT") {
        using LDLT = Eigen::SimplicialLDLT<ZSparseMatrix::ParentType>;
        return SparseSolver::Ptr(new SparseSolverImplementation<LDLT>);
    }
    if (solver_type == "LLT") {
        using LLT = Eigen::SimplicialLLT<ZSparseMatrix::ParentType>;
        return SparseSolver::Ptr(new SparseSolverImplementation<LLT>);
    }
    if (solver_type == "CG") {
        using CG = Eigen::ConjugateGradient<ZSparseMatrix::ParentType>;
        return SparseSolver::Ptr(new SparseSolverImplementation<CG>);
    }
    if (solver_type == "LSCG") {
        using LSCG = Eigen::LeastSquaresConjugateGradient<ZSparseMatrix::ParentType>;
        return SparseSolver::Ptr(new SparseSolverImplementation<LSCG>);
    }
    if (solver_type == "BiCG") {
        using BiCGSTAB = Eigen::BiCGSTAB<ZSparseMatrix::ParentType>;
        return SparseSolver::Ptr(new SparseSolverImplementation<BiCGSTAB>);
    }
    if (solver_type == "SparseLU") {
        using SparseLU = Eigen::SparseLU<ZSparseMatrix::ParentType,
              Eigen::COLAMDOrdering<ZSparseMatrix::ParentType::StorageIndex> >;
        return SparseSolver::Ptr(new SparseSolverImplementation<SparseLU>);
    }
    if (solver_type == "SparseQR") {
        using SparseQR = Eigen::SparseQR<ZSparseMatrix::ParentType,
              Eigen::COLAMDOrdering<ZSparseMatrix::ParentType::StorageIndex> >;
        return SparseSolver::Ptr(new SparseSolverImplementation<SparseQR>);
    }
#ifdef WITH_UMFPACK
    if (solver_type == "UmfPackLU") {
        using UmfPackLU = Eigen::UmfPackLU<ZSparseMatrix::ParentType>;
        return SparseSolver::Ptr(new SparseSolverImplementation<UmfPackLU>);
    }
#endif
#ifdef WITH_CHOLMOD
    if (solver_type == "Cholmod") {
        using CholmodSupernodalLLT =
            Eigen::CholmodSupernodalLLT<ZSparseMatrix::ParentType>;
        return SparseSolver::Ptr(
                new SparseSolverImplementation<CholmodSupernodalLLT>);
    }
#endif

    std::stringstream err_msg;
    err_msg << "Unsupported solver type " << solver_type;
    throw NotImplementedError(err_msg.str());
}
