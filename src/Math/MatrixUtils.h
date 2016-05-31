/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <vector>

namespace PyMesh {
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

    /**
     * Reinterpret data as matrix of a different shape.
     *
     * This method does no copy on its own, however, copying might be
     * performed during casting such as the following:
     *
     *      VectorF data = VectorF::Ones(4);
     *      MatrixFr m = reshape<MatrixFr>(data, 2, 2);
     * 
     * The copying operation occurs at the assignment because a type cast is
     * needed.  To avoid copying, do the following:
     *
     *      auto m = reshape<MatrixFr>(data, 2, 2);
     *
     * Of course, it is the user's responsibility to ensure the memory shared
     * by data and m is valid throughout the life time of m.  In particular,
     * deleting data will likely to invalidate m.
     */
    template<typename T, typename Derived>
    Eigen::Map<T> reshape(Eigen::MatrixBase<Derived>& M, size_t rows, size_t cols);

    /**
     * Flatten a matrix into vector according to its storage order.
     *
     * See the documentation of flatten above on issues about copying.
     */
    template<typename T, typename Derived>
    Eigen::Map<T> flatten(Eigen::MatrixBase<Derived>& matrix);

    template<typename T, typename Derived>
    Eigen::Map<const T> flatten(const Eigen::MatrixBase<Derived>& matrix);

    /**
     * Return [0, ..., n-1] in type T.  Expect T(n) and T[i] to work exactly as
     * std::vector.
     */
    template<typename T>
    T range(const size_t n);
}
}

#include "MatrixUtils.inl"
