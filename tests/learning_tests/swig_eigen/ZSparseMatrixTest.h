#pragma once
#include <vector>
#include <Eigen/Sparse>

#include <Core/EigenTypedef.h>

#include "ZSparseMatrix.h"

class ZSparseMatrixTest {
    public:
        typedef Eigen::Triplet<Float> T;
        ZSparseMatrixTest() : m_matrix(6, 6){
            std::vector<T> triplets;
            for (size_t i=0; i<6; i++) {
                triplets.push_back(T(i, i, i));
            }
            m_matrix.setFromTriplets(triplets.begin(), triplets.end());
        }

        ZSparseMatrix get_matrix() {
            return m_matrix;
        }

    private:
        ZSparseMatrix m_matrix;
};
