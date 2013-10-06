#pragma once
#include <vector>
#include <algorithm>

template<typename T>
class IndexHeap {
    public:
        class IndexComp {
            public:
                IndexComp(const IndexHeap<T>* host, bool max_heap) : m_host(host), m_max_heap(max_heap) {}
                bool operator()(size_t i, size_t j) const {
                    if (m_max_heap)
                        return m_host->m_data[i] < m_host->m_data[j];
                    else
                        return m_host->m_data[i] > m_host->m_data[j];
                }
            private:
                const IndexHeap<T>* m_host;
                bool m_max_heap;
        };

    public:
        IndexHeap(const std::vector<T>& data, bool max_heap = true) : m_data(data), m_comp(this, max_heap) {
            m_indices.resize(m_data.size());
            for (size_t i=0; i<m_data.size(); i++) { m_indices[i] = i; }

            std::make_heap(m_indices.begin(), m_indices.end(), m_comp);
        }

    public:
        size_t top() const {
            return m_indices.front();
        }

        void pop() {
            std::pop_heap(m_indices.begin(), m_indices.end(), m_comp);
            m_indices.pop_back();
        }

        size_t size() const {
            return m_indices.size();
        }

        bool empty() const {
            return m_indices.empty();
        }

    private:
        const std::vector<T>& m_data;
        std::vector<size_t> m_indices;
        IndexComp m_comp;
};
