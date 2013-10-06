#pragma once

class Edge {
    public:
        Edge() : m_v1(0), m_v2(0) {}
        Edge(size_t i, size_t j) {
            if (i<j) {
                m_v1 = i;
                m_v2 = j;
            } else {
                m_v1 = j;
                m_v2 = i;
            }
        }

        bool operator==(const Edge& other) const {
            return (m_v1 == other.m_v1) && (m_v2 == other.m_v2);
        }

        bool operator<(const Edge& other) const {
            if (m_v1 != other.m_v1) {
                return m_v1 < other.m_v1;
            } else {
                return m_v2 < other.m_v2;
            }
        }

        int hash() const {
            // Magic primes (p1, p2, p3) from the following paper.
            // "Optimized Spacial Hashing for Collision Detection of
            // Deformable Objects" by Teschner et al. (VMV 2003)
            const int p1 = 73856093;
            const int p2 = 19349663;
            const int p3 = 83492791;
            return (m_v1*p1) ^ (m_v2*p2);
        }

    public:
        size_t m_v1;
        size_t m_v2;
};
