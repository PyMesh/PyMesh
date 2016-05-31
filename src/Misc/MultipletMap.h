/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <limits>
#include <vector>

#include <unordered_map>

#include <Core/Exception.h>
#include <Misc/Multiplet.h>

namespace PyMesh {

template <typename T>
class MultipletMap {
    public:
        struct MultipletHashFunc {
            int operator() (const Multiplet& trip) const {
                return trip.hash();
            }
        };

        typedef std::vector<T> ValueType;
        typedef std::unordered_map<Multiplet, ValueType, MultipletHashFunc> MultipletHashMap;
        typedef typename MultipletHashMap::iterator iterator;
        typedef typename MultipletHashMap::const_iterator const_iterator;

    public:
        MultipletMap() {
            int infinity = std::numeric_limits<int>::max();
            //m_map.set_empty_key(Multiplet(infinity, infinity, infinity));
        }

        void insert(const Multiplet& t, T val) {
            iterator itr = m_map.find(t);
            if (itr == m_map.end()) {
                ValueType item;
                item.push_back(val);
                m_map[t] = item;
            } else {
                m_map[t].push_back(val);
            }
        }

        ValueType& operator[] (const Multiplet& t) {
            return m_map[t];
        }

        const ValueType& get(const Multiplet& t) const {
            const_iterator itr = m_map.find(t);
            if (itr == m_map.end())
                throw RuntimeError("Key not found");
            return itr->second;
        }

        iterator find(const Multiplet& t) {
            return m_map.find(t);
        }

        const_iterator find(const Multiplet& t) const {
            return m_map.find(t);
        }

        void clear() {
            m_map.clear();
        }

        bool empty() const {
            return m_map.empty();
        }

        size_t size() const {
            return m_map.size();
        }

    public:
        iterator begin() { return m_map.begin(); }
        const_iterator begin() const { return m_map.begin(); }

        iterator end() { return m_map.end(); }
        const_iterator end() const { return m_map.end(); }

    private:
        MultipletHashMap m_map;
};

}
