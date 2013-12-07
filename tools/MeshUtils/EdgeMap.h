#pragma once

#include <vector>
#include <limits>
#include <google/dense_hash_map>

#include <Core/Exception.h>

#include "Edge.h"

template <typename T>
class EdgeMap {
    public:
        struct EdgeHashFunc {
            int operator() (const Edge& edge) const {
                return edge.hash();
            }
        };

        typedef std::vector<T> ValueType;
        typedef google::dense_hash_map<Edge, ValueType, EdgeHashFunc> EdgeHashMap;
        typedef typename EdgeHashMap::iterator iterator;
        typedef typename EdgeHashMap::const_iterator const_iterator;

    public:
        EdgeMap() {
            size_t infinity = std::numeric_limits<size_t>::max();
            m_map.set_empty_key(Edge(infinity-1, infinity));
        }

        void insert(const Edge& e, T val) {
            typename EdgeHashMap::iterator itr = m_map.find(e);
            if (itr == m_map.end()) {
                ValueType item;
                item.push_back(val);
                m_map[e] = item;
            } else {
                m_map[e].push_back(val);
            }
        }

        ValueType& operator[] (const Edge& e) {
            return m_map[e];
        }

        const ValueType& get(const Edge& e) const {
            const_iterator itr = m_map.find(e);
            if (itr == m_map.end())
                throw RuntimeError("Key not found.");
            return itr->second;
        }


        void clear() {
            m_map.clear();
        }


    public:
        iterator& begin() { return m_map.begin(); }
        const_iterator& begin() const { return m_map.begin(); }

        iterator& end() { return m_map.end(); }
        const_iterator& end() const { return m_map.end(); }

    private:
        EdgeHashMap m_map;
};
