/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <limits>
#include <vector>

#include <unordered_map>

#include <Core/Exception.h>
#include <Misc/Multiplet.h>

namespace PyMesh {

template <typename KeyType, typename T>
class MultipletMap {
    public:
        struct MultipletHashFunc {
            inline int operator() (const KeyType& key) const {
                return key.hash();
            }
        };

        using ValueType = std::vector<T>;
        using MultipletHashMap = std::unordered_map<KeyType, ValueType, MultipletHashFunc>;
        using iterator = typename MultipletHashMap::iterator;
        using const_iterator = typename MultipletHashMap::const_iterator;

    public:
        MultipletMap() = default;

        void insert(const KeyType& t, T val) {
            iterator itr = m_map.find(t);
            if (itr == m_map.end()) {
                m_map.insert({t, {val}});
            } else {
                m_map[t].push_back(val);
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
