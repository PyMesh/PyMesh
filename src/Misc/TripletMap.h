/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <limits>
#include <vector>

//#include <google/dense_hash_map>
#include <unordered_map>

#include <Core/Exception.h>
#include <Misc/Triplet.h>

namespace PyMesh {

template <typename T>
class TripletMap {
    public:
        struct TripletHashFunc {
            int operator() (const Triplet& trip) const {
                return trip.hash();
            }
        };

        typedef std::vector<T> ValueType;
        typedef std::unordered_map<Triplet, ValueType, TripletHashFunc> TripletHashMap;
        typedef typename TripletHashMap::iterator iterator;
        typedef typename TripletHashMap::const_iterator const_iterator;

    public:
        TripletMap() {
            int infinity = std::numeric_limits<int>::max();
            //m_map.set_empty_key(Triplet(infinity, infinity, infinity));
        }

        void insert(const Triplet& t, T val) {
            iterator itr = m_map.find(t);
            if (itr == m_map.end()) {
                ValueType item;
                item.push_back(val);
                m_map[t] = item;
            } else {
                m_map[t].push_back(val);
            }
        }

        ValueType& operator[] (const Triplet& t) {
            return m_map[t];
        }

        const ValueType& get(const Triplet& t) const {
            const_iterator itr = m_map.find(t);
            if (itr == m_map.end())
                throw RuntimeError("Key not found");
            return itr->second;
        }

        iterator find(const Triplet& t) {
            return m_map.find(t);
        }

        const_iterator find(const Triplet& t) const {
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
        TripletHashMap m_map;
};

}
