/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include <algorithm>
#include <Core/EigenTypedef.h>

namespace PyMesh {

class Triplet {
    public:
        Triplet(int v1=0, int v2=0, int v3=0) : m_ori_data(v1, v2, v3) {
            int v_max = m_ori_data.maxCoeff();
            int v_min = m_ori_data.minCoeff();
            int v_mid = v1 + v2 + v3 - v_max - v_min;
            m_data.coeffRef(0) = v_max;
            m_data.coeffRef(1) = v_mid;
            m_data.coeffRef(2) = v_min;
        }

        bool operator==(const Triplet& other) const {
            return (m_data.coeff(0) == other.m_data.coeff(0) &&
                    m_data.coeff(1) == other.m_data.coeff(1) &&
                    m_data.coeff(2) == other.m_data.coeff(2));
        }

        bool operator<(const Triplet& other) const {
            if (m_data.coeff(0) < other.m_data.coeff(0)) return true;
            else if (m_data.coeff(0) > other.m_data.coeff(0)) return false;

            if (m_data.coeff(1) < other.m_data.coeff(1)) return true;
            else if (m_data.coeff(1) > other.m_data.coeff(1)) return false;

            if (m_data.coeff(2) < other.m_data.coeff(2)) return true;
            else return false;
        }

        const Vector3I& get_data() const { return m_data; }
        const Vector3I& get_ori_data() const { return m_ori_data; }

        int hash() const {
            // Magic primes (p1, p2, p3) from the following paper.
            // "Optimized Spacial Hashing for Collision Detection of
            // Deformable Objects" by Teschner et al. (VMV 2003)
            const int p1 = 73856093;
            const int p2 = 19349663;
            const int p3 = 83492791;
            return (m_data.coeff(0)*p1) ^ (m_data.coeff(1)*p2) ^ (m_data.coeff(2)*p3);
        }

    private:
        Vector3I m_data;
        Vector3I m_ori_data;
};

}
