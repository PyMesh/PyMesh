/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "SparseSolverFactory.h"
#include <sstream>

#include <Eigen/CholmodSupport>
#include <Eigen/IterativeLinearSolvers>
#include <Eigen/SparseCholesky>
#include <Eigen/SparseLU>
#include <Eigen/SparseQR>
#include <Eigen/UmfPackSupport>
#include <third_party/SparseMatrices.hh>

#include <Core/Exception.h>

#include "SparseSolverImplementation.h"

using namespace PyMesh;

SparseSolverFactory::SparseSolverFactory(const std::string& solver_type) {
    if (solver_type == "LDLT") {
        m_solver = SparseSolverPtr(
                new SparseSolverImplementation<Eigen::SimplicialLDLT<ZSparseMatrix::ParentType> >);
    } else if (solver_type == "LLT") {
        m_solver = SparseSolverPtr(
                new SparseSolverImplementation<Eigen::SimplicialLLT<ZSparseMatrix::ParentType> >);
    } else if (solver_type == "CG") {
        m_solver = SparseSolverPtr(
                new SparseSolverImplementation<Eigen::ConjugateGradient<ZSparseMatrix::ParentType> >);
    } else if (solver_type == "BiCG") {
        m_solver = SparseSolverPtr(
                new SparseSolverImplementation<Eigen::BiCGSTAB<ZSparseMatrix::ParentType> >);
    } else if (solver_type == "SparseLU") {
        m_solver = SparseSolverPtr(
                new SparseSolverImplementation<Eigen::SparseLU<ZSparseMatrix::ParentType> >);
    } else if (solver_type == "SparseQR") {
        m_solver = SparseSolverPtr(
                new SparseSolverImplementation<Eigen::SparseQR<ZSparseMatrix::ParentType,
                Eigen::COLAMDOrdering<ZSparseMatrix::ParentType::Index> > >);
    } else if (solver_type == "UmfPackLU") {
        m_solver = SparseSolverPtr(
                new SparseSolverImplementation<Eigen::UmfPackLU<ZSparseMatrix::ParentType> >);
    } else if (solver_type == "Cholmod") {
        m_solver = SparseSolverPtr(
                new SparseSolverImplementation<Eigen::CholmodSupernodalLLT<ZSparseMatrix::ParentType> >);
    //} else if (solver_type == "UmfPack") {
    //    m_solver = SparseSolverPtr(
    //            new SparseSolverImplementation<UmfpackFactorizer>);
    } else {
        std::stringstream err_msg;
        err_msg << "Unsupported solver type " << solver_type;
        throw NotImplementedError(err_msg.str());
    }
}
