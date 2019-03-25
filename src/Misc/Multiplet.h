/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include <algorithm>
#include <functional>
#include <Core/EigenTypedef.h>
#include <Core/Exception.h>

namespace PyMesh {

/**
 * Multiplet generalized the concept of combination.
 * i.e. the same set items but with different orders are considered equal.
 * In contrast with std::set, The orignal order is also remembered.
 */
template<typename T, int Dim>
class Multiplet {
    public:
        using Data = Eigen::Matrix<T, Dim, 1>;

        Multiplet() = default;
        Multiplet(const Data&& items) : m_ori_data(items) {
            init_data();
        }

        virtual ~Multiplet() = default;

    public:

        inline bool operator==(const Multiplet<T, Dim>& other) const {
            return m_data == other.m_data;
        }

        bool operator<(const Multiplet<T, Dim>& other) const {
            for (size_t i=0; i<Dim; i++) {
                if (m_data[i] < other.m_data[i]) return true;
                else if (m_data[i] > other.m_data[i]) return false;
            }
            // this == other
            return false;
        }

        inline const Data& get_data() const { return m_data; }
        inline const Data& get_ori_data() const { return m_ori_data; }

        virtual int hash() const =0;

    private:
        void init_data() {
            // Sort m_ori_data into descending order.
            m_data = m_ori_data;
            std::sort(m_data.data(), m_data.data() + Dim, std::greater<T>());
        }

    protected:
        Data m_data;
        Data m_ori_data;
};

class Singleton : public Multiplet<int, 1> {
    public:
        using Parent = Multiplet<int, 1>;
        Singleton(int v1=0) {
            Parent::m_ori_data << v1;
            Parent::m_data << v1;
        }
        virtual ~Singleton() = default;

        inline virtual int hash() const {
            // Use identity hash.
            return Parent::m_data[0];
        }
};

class Duplet : public Multiplet<int, 2> {
    public:
        using Parent = Multiplet<int, 2>;
        Duplet(int v1=0, int v2=0) {
            Parent::m_ori_data << v1, v2;
            if (v1 >= v2) {
                Parent::m_data << v1, v2;
            } else {
                Parent::m_data << v2, v1;
            }
        }
        virtual ~Duplet() = default;

        inline virtual int hash() const {
            // Magic primes (p1, p2) from the following paper.
            // "Optimized Spacial Hashing for Collision Detection of
            // Deformable Objects" by Teschner et al. (VMV 2003)
            constexpr int p1 = 73856093;
            constexpr int p2 = 19349663;
            return (Parent::m_data.coeff(0)*p1) ^ (Parent::m_data.coeff(1)*p2);
        }
};

class Triplet : public Multiplet<int, 3> {
    public:
        using Parent = Multiplet<int, 3>;
        Triplet(int v1=0, int v2=0, int v3=0) : Parent({v1, v2, v3}) {}
        virtual ~Triplet() = default;

        inline virtual int hash() const {
            // Magic primes (p1, p2, p3) from the following paper.
            // "Optimized Spacial Hashing for Collision Detection of
            // Deformable Objects" by Teschner et al. (VMV 2003)
            constexpr int p1 = 73856093;
            constexpr int p2 = 19349663;
            constexpr int p3 = 83492791;
            return (Parent::m_data.coeff(0)*p1) ^
                (Parent::m_data.coeff(1)*p2) ^
                (Parent::m_data.coeff(2)*p3);
        }
};

class Quadruplet : public Multiplet<int, 4> {
    public:
        using Parent = Multiplet<int, 4>;
        Quadruplet(int v1=0, int v2=0, int v3=0, int v4=0) : Parent({v1, v2, v3, v4}) {}
        virtual ~Quadruplet() = default;

        inline virtual int hash() const {
            // Magic primes (p1, p2, p3) from the following paper.
            // "Optimized Spacial Hashing for Collision Detection of
            // Deformable Objects" by Teschner et al. (VMV 2003)
            constexpr int p1 = 73856093;
            constexpr int p2 = 19349663;
            constexpr int p3 = 83492791;
            constexpr int p4 = 100663319;
            return (Parent::m_data.coeff(0)*p1) ^
                (Parent::m_data.coeff(1)*p2) ^
                (Parent::m_data.coeff(2)*p3) ^
                (Parent::m_data.coeff(4)*p4);
        }
};

template<typename KeyType>
struct MultipletHashFunc {
    inline int operator() (const KeyType& key) const {
        return key.hash();
    }
};

}
