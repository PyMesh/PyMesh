/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include <algorithm>
#include <cassert>
#include <iostream>
#include <initializer_list>

#include <Core/EigenTypedef.h>
#include <Core/Exception.h>

namespace PyMesh {

template <typename T, int dim>
class VectorHashKey {
    public:
        typedef T ValueType;
        typedef Eigen::Matrix<ValueType, dim, 1> VectorType;
    public:
        VectorHashKey() : m_data(VectorType::Zero(dim)) {}
        VectorHashKey(const VectorType& coord) : m_data(coord) {}
        VectorHashKey(ValueType v) : m_data(VectorType::Ones(dim) * v) {}
        VectorHashKey(std::initializer_list<T> args) {
            assert(args.size() == dim);
            std::copy(args.begin(), args.end(), m_data.data());
        }
        virtual ~VectorHashKey() {}

        bool operator==(const VectorHashKey& other) const {
            return m_data == other.m_data;
        }

        ValueType operator[](size_t i) const {
            assert(i<dim);
            return m_data[i];
        }

        virtual size_t hash() const {
            return hash(m_data);
        }

        size_t hash(const Eigen::Matrix<ValueType, 2, 1>&) const {
            return m_data[0]*p1 ^ m_data[1]*p2;
        }

        size_t hash(const Eigen::Matrix<ValueType, 3, 1>&) const {
            return m_data[0]*p1 ^ m_data[1]*p2 ^ m_data[2]*p3;
        }

        const VectorType get_raw_data() const {
            return m_data;
        }

    protected:
        VectorType m_data;

        // Magic primes (p1, p2, p3) from the following paper.
        // "Optimized Spacial Hashing for Collision Detection of
        // Deformable Objects" by Teschner et al. (VMV 2003)
        //
        // Not that p2 is actaully not a prime!  p2 = 41 * 471943
        // Sigh, graphics paper cannot be trusted!
        static const long p1 = 73856093;
        static const long p2 = 19349663;
        static const long p3 = 83492791;
};

}
