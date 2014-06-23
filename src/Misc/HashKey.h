#pragma once
#include <cassert>
#include <iostream>

#include <Core/EigenTypedef.h>

template <typename T, int dim>
class VectorHashKey {
    public:
        typedef T ValueType;
        typedef Eigen::Matrix<ValueType, dim, 1> VectorType;
    public:
        VectorHashKey() : m_data(VectorType::Zero(dim)) {}
        VectorHashKey(const VectorType& coord) : m_data(coord) {}
        VectorHashKey(ValueType v) : m_data(VectorType::Ones(dim) * v) {}

        bool operator==(const VectorHashKey& other) const {
            return m_data == other.m_data;
        }

        ValueType operator[](size_t i) const {
            assert(i<dim);
            return m_data[i];
        }

        virtual size_t hash() const =0;

    protected:
        VectorType m_data;
};

class Vector3IHashKey : public VectorHashKey<int, 3> {
    public:
        Vector3IHashKey() : VectorHashKey<int, 3>(VectorType::Zero(3)) {}

        Vector3IHashKey(int x, int y=0, int z=0) :
            VectorHashKey<int, 3>(Vector3I(x,y,z)) { }

        virtual size_t hash() const {
            // Magic primes (p1, p2, p3) from the following paper.
            // "Optimized Spacial Hashing for Collision Detection of
            // Deformable Objects" by Teschner et al. (VMV 2003)
            const int p1 = 73856093;
            const int p2 = 19349663;
            const int p3 = 83492791;
            return m_data[0]*p1 ^ m_data[1]*p2 ^ m_data[2]*p3;
        }
};

class Vector3FHashKey : public VectorHashKey<Float, 3> {
    public:
        Vector3FHashKey() : VectorHashKey<Float, 3>(VectorType::Zero(3)) {}

        Vector3FHashKey(Float x, Float y=0, Float z=0) :
            VectorHashKey<Float, 3>(Vector3F(x,y,z)) { }

        virtual size_t hash() const {
            // Magic primes (p1, p2, p3) from the following paper.
            // "Optimized Spacial Hashing for Collision Detection of
            // Deformable Objects" by Teschner et al. (VMV 2003)
            const int p1 = 73856093;
            const int p2 = 19349663;
            const int p3 = 83492791;

            return 
                size_t(m_data[0]*p1) ^
                size_t(m_data[1]*p2) ^ 
                size_t(m_data[2]*p3);
        }
};
