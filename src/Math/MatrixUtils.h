#pragma once

#include <vector>

namespace MatrixUtils {
    /**
     * This method stack multiple matrices into a single matrix by joining rows.
     * T = MatrixF or MatrixI or MatrixFr or MatrixIr
     *
     * If T = VectorF or VectorI, the output is still a 1D vector with all
     * entries joined.  Use the mehtod rowstack or colstack to convert arrow of
     * vectors into matrices.
     */
    template<typename T>
    T vstack(const std::vector<T>& matrices);

    template<typename T>
    Eigen::Matrix<typename T::Scalar, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>
    rowstack(const std::vector<T>& rows);

    template<typename T>
    Eigen::Matrix<typename T::Scalar, Eigen::Dynamic, Eigen::Dynamic, Eigen::ColMajor>
    colstack(const std::vector<T>& cols);

    /**
     * This method convert a std::vector of scalars (T=int or double) into a
     * column vector (VectorI, VectorF).
     */
    template<typename T>
    Eigen::Matrix<T, Eigen::Dynamic, 1> std2eigen(const std::vector<T>& data);
}

#include "MatrixUtils.inl"
