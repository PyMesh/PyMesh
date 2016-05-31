/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <iostream>
#include <sstream>
#include <Core/EigenTypedef.h>
#include <Core/Exception.h>

namespace PyMesh {

template<size_t FROM_DIM, size_t TO_DIM>
class DimReduction {
    public:
        typedef Eigen::Matrix<Float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> MatrixIn;
        typedef Eigen::Matrix<Float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> MatrixOut;
        DimReduction(const MatrixIn& pts) {
            const Float rel_eps = 1e-3;
            m_mean = pts.colwise().sum().array() / pts.rows();
            Eigen::JacobiSVD<MatrixIn> svd(
                    pts.rowwise() - m_mean.transpose(), Eigen::ComputeThinV);
            auto singular_vals = svd.singularValues();
            size_t rank = 0;
            for (size_t i=0; i<FROM_DIM; i++) {
                if (singular_vals[i] > singular_vals[0] * rel_eps) {
                    rank++;
                }
            }
            //size_t rank = svd.rank();
            if (rank > TO_DIM) {
                std::stringstream err_msg;
                err_msg << "Data is of dimention " << rank
                    << ", cannot be reduced to dimentions " << TO_DIM
                    << std::endl;
                err_msg << "Singular values: " << singular_vals.transpose() << std::endl;
                throw RuntimeError(err_msg.str());
            }
            
            const MatrixF& V = svd.matrixV();
            m_basis = V.block(0, 0, V.rows(), TO_DIM);
        }

    public:
        MatrixOut project(const MatrixIn& pts) {
            return (pts.rowwise() - m_mean.transpose()) * m_basis;
        }

        MatrixIn  unproject(const MatrixOut& pts) {
            return (m_basis * pts.transpose()).transpose().rowwise() + m_mean.transpose();
        }

    private:
        VectorF m_mean;
        MatrixF m_basis;
};

}
