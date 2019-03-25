/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <limits>
#include <mutex>
#include <vector>
#include <tbb/concurrent_vector.h>

#include <tbb/concurrent_unordered_map.h>

#include <Core/Exception.h>
#include <Misc/Multiplet.h>

namespace PyMesh {

template <typename KeyType, typename T>
class MultipletMap {
    public:
        //using ValueType = std::vector<T>;
        using ValueType = tbb::concurrent_vector<T>;
        using MultipletHashMap = tbb::concurrent_unordered_map<
            KeyType, ValueType, MultipletHashFunc<KeyType>>;
        using iterator = typename MultipletHashMap::iterator;
        using const_iterator = typename MultipletHashMap::const_iterator;
        using range_type = typename MultipletHashMap::range_type;
        using const_range_type = typename MultipletHashMap::const_range_type;

    public:
        MultipletMap() = default;

        void insert(const KeyType& t, T val) {
            iterator itr = m_map.find(t);
            if (itr == m_map.end()) {
                m_map.insert({t, {val}});
            } else {
                itr->second.emplace_back(val);
            }
        }

        ValueType& operator[] (const KeyType& t) {
            return m_map[t];
        }

        const ValueType& get(const KeyType& t) const {
            const_iterator itr = m_map.find(t);
            if (itr == m_map.end())
                throw RuntimeError("Key not found");
            return itr->second;
        }

        iterator find(const KeyType& t) {
            return m_map.find(t);
        }

        const_iterator find(const KeyType& t) const {
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

        range_type range() { return m_map.range(); }
        const_range_type range() const { return m_map.range(); }

    private:
        MultipletHashMap m_map;
};

template<typename T>
using SingletonMap = MultipletMap<Singleton, T>;
template<typename T>
using DupletMap = MultipletMap<Duplet, T>;
template<typename T>
using TripletMap = MultipletMap<Triplet, T>;
template<typename T>
using QaudrupletMap = MultipletMap<Quadruplet, T>;

}
