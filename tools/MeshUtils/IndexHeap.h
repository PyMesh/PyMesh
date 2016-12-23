/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include <cassert>
#include <vector>
#include <algorithm>

namespace PyMesh {

template<typename T>
class IndexHeap {
    public:
        IndexHeap(bool max_heap = true) {
            init_comp(max_heap);
        }

        IndexHeap(const std::vector<T>& data, bool max_heap = true) {
            init_comp(max_heap);
            init(data);
        }

    public:
        void init_comp(bool max_heap) {
            if (max_heap) {
                m_comp = [this](size_t i, size_t j) {
                    return this->m_data[i] < this->m_data[j];
                };
            } else {
                m_comp = [this](size_t i, size_t j) {
                    return this->m_data[i] > this->m_data[j];
                };
            }
        }

        void init(const std::vector<T>& data) {
            m_data = data;
            size_t data_size = m_data.size();
            m_indices.resize(data_size);
            for (size_t i=0; i<data_size; i++) { m_indices[i] = i; }
            std::make_heap(m_indices.begin(), m_indices.end(), m_comp);
        }

        size_t top() const {
            assert(!m_indices.empty());
            assert(m_indices.front() < m_data.size());
            return m_indices.front();
        }
        
        T top_value() const {
            return m_data[top()];
        }

        void pop() {
            std::pop_heap(m_indices.begin(), m_indices.end(), m_comp);
            m_indices.pop_back();
        }

        void push(T val) {
            size_t idx = m_data.size();
            m_data.push_back(val);
            m_indices.push_back(idx);
            std::push_heap(m_indices.begin(), m_indices.end(), m_comp);
        }

        void update(size_t index, T val) {
            m_data[index] = val;
            std::make_heap(m_indices.begin(), m_indices.end(), m_comp);
        }

        size_t size() const {
            return m_indices.size();
        }

        bool empty() const {
            return m_indices.empty();
        }

    private:
        std::vector<T> m_data;
        std::vector<size_t> m_indices;
        std::function<bool(size_t, size_t)> m_comp;
};
}
