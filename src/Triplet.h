#pragma once
#include <algorithm>
#include "EigenTypedef.h"

class Triplet {
    public:
        Triplet(int v1, int v2, int v3) {
            int v_max = std::max(std::max(v1, v2), v3);
            int v_min = std::min(std::min(v1, v2), v3);
            int v_mid = v1 + v2 + v3 - v_max - v_min;
            m_data[0] = v_max;
            m_data[1] = v_mid;
            m_data[2] = v_min;
            m_ori_data[0] = v1;
            m_ori_data[1] = v2;
            m_ori_data[2] = v3;
        }

        bool operator==(const Triplet& other) const {
            return (m_data[0] == other.m_data[0] &&
                    m_data[1] == other.m_data[1] &&
                    m_data[2] == other.m_data[2]);
        }

        bool operator<(const Triplet& other) const {
            if (m_data[0] < other.m_data[0]) return true;
            else if (m_data[0] > other.m_data[0]) return false;

            if (m_data[1] < other.m_data[1]) return true;
            else if (m_data[1] > other.m_data[1]) return false;

            if (m_data[2] < other.m_data[2]) return true;
            else return false;
        }

        const Vector3I& get_data() const { return m_data; }
        const Vector3I& get_ori_data() const { return m_ori_data; }

    private:
        Vector3I m_data;
        Vector3I m_ori_data;
};
