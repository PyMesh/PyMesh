/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include <algorithm>
#include <Core/EigenTypedef.h>
#include <Core/Exception.h>

namespace PyMesh {

/**
 * Multiplet generalized the concept of combination.
 * i.e. the same set items but with different orders are considered equal.
 * In contrast with std::set, The orignal order is also remembered.
 */
class Multiplet {
    public:
        Multiplet(const VectorI& items) : m_ori_data(items) {
            init_data();
        }

        Multiplet(int v1=0, int v2=0 ,int v3=0, int v4=0) {
            // Special case of quadruplet
            m_ori_data.resize(4);
            m_ori_data << v1, v2, v3, v4;
            init_data();
        }

        bool operator==(const Multiplet& other) const {
            return m_data == other.m_data;
        }

        bool operator<(const Multiplet& other) const {
            const size_t num_items = m_data.size();
            for (size_t i=0; i<num_items; i++) {
                if (m_data[i] < other.m_data[i]) return true;
                else if (m_data[i] > other.m_data[i]) return false;
            }
            // this == other
            return false;
        }

        const VectorI& get_data() const { return m_data; }
        const VectorI& get_ori_data() const { return m_ori_data; }

        int hash() const {
            if (m_data.size() != 4) {
                throw NotImplementedError(
                        "Hash computation is not supported for Multiplet.");
            }
            const int primes[] = {
                // Magic primes from http://planetmath.org/goodhashtableprimes
                402653189,
                805306457,
                201326611,
                100663319
            };
            int r = (m_data.coeff(0)*primes[0]) ^ 
                    (m_data.coeff(1)*primes[1]) ^ 
                    (m_data.coeff(2)*primes[2]) ^ 
                    (m_data.coeff(3)*primes[3]);
            return r;
        }

    private:
        void init_data() {
            // Sort m_ori_data into descending order.
            m_data = m_ori_data;
            std::sort(m_data.data(), m_data.data() + m_data.size());
            m_data = m_data.reverse();
        }

    private:
        VectorI m_data;
        VectorI m_ori_data;
};

}
