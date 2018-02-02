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
#ifdef WITH_MKL
#include <Eigen/PardisoSupport>
#endif

#include <Core/Exception.h>

#include "SparseSolverImplementation.h"

using namespace PyMesh;

SparseSolver::Ptr SparseSolver::create(const std::string& solver_type) {
    if (solver_type == "LDLT") {
        using LDLT = Eigen::SimplicialLDLT<ZSparseMatrix::ParentType>;
        return SparseSolver::Ptr(new SparseSolverImplementation<LDLT>);
    } else if (solver_type == "LLT") {
        using LLT = Eigen::SimplicialLLT<ZSparseMatrix::ParentType>;
        return SparseSolver::Ptr(new SparseSolverImplementation<LLT>);
    } else if (solver_type == "CG") {
        using CG = Eigen::ConjugateGradient<ZSparseMatrix::ParentType,
              Eigen::Lower|Eigen::Upper>;
        return SparseSolver::Ptr(
                new IterativeSparseSolverImplementation<CG>);
    } else if (solver_type == "LSCG") {
        using LSCG = Eigen::LeastSquaresConjugateGradient<ZSparseMatrix::ParentType>;
        return SparseSolver::Ptr(
                new IterativeSparseSolverImplementation<LSCG>);
    } else if (solver_type == "BiCG") {
        using BiCGSTAB = Eigen::BiCGSTAB<ZSparseMatrix::ParentType>;
        return SparseSolver::Ptr(
                new IterativeSparseSolverImplementation<BiCGSTAB>);
    } else if (solver_type == "SparseLU") {
        using SparseLU = Eigen::SparseLU<ZSparseMatrix::ParentType,
              Eigen::COLAMDOrdering<ZSparseMatrix::ParentType::StorageIndex> >;
        return SparseSolver::Ptr(new SparseSolverImplementation<SparseLU>);
    } else if (solver_type == "SparseQR") {
        using SparseQR = Eigen::SparseQR<ZSparseMatrix::ParentType,
              Eigen::COLAMDOrdering<ZSparseMatrix::ParentType::StorageIndex> >;
        return SparseSolver::Ptr(new SparseSolverImplementation<SparseQR>);
    }
#ifdef WITH_UMFPACK
    if (solver_type == "UmfPackLU") {
        using UmfPackLU = Eigen::UmfPackLU<ZSparseMatrix::ParentType>;
        return SparseSolver::Ptr(
                new SparseSolverImplementationWithLocalCopy<UmfPackLU>);
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
#ifdef WITH_MKL
    if (solver_type == "PardisoLLT") {
        using PardisoLLT = Eigen::PardisoLLT<ZSparseMatrix::ParentType>;
        return SparseSolver::Ptr(
                new SparseSolverImplementation<PardisoLLT>);
    } else if (solver_type == "PardisoLDLT") {
        using PardisoLDLT = Eigen::PardisoLDLT<ZSparseMatrix::ParentType>;
        return SparseSolver::Ptr(
                new SparseSolverImplementation<PardisoLDLT>);
    } else if (solver_type == "PardisoLU") {
        using PardisoLU = Eigen::PardisoLU<ZSparseMatrix::ParentType>;
        return SparseSolver::Ptr(
                new SparseSolverImplementation<PardisoLU>);
    }
#endif

    std::stringstream err_msg;
    err_msg << "Unsupported solver type " << solver_type;
    throw NotImplementedError(err_msg.str());
}

std::vector<std::string> SparseSolver::get_supported_solvers() {
    std::vector<std::string> solver_names{
        "LDLT",
        "LLT",
        "CG",
        "LSCG",
        "BiCG",
        "SparseLU",
        "SparseQR"
    };
#ifdef WITH_UMFPACK
    solver_names.push_back("UmfPackLU");
#endif
#ifdef WITH_CHOLMOD
    solver_names.push_back("Cholmod");
#endif
#ifdef WITH_MKL
    solver_names.push_back("PardisoLLT");
    solver_names.push_back("PardisoLDLT");
    solver_names.push_back("PardisoLU");
#endif
    return solver_names;
}
