#pragma once
#include <algorithm>
#include <Core/EigenTypedef.h>
#include <Core/Exception.h>

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
            throw NotImplementedError(
                    "Hash computation is not supported for Multiplet.");
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
