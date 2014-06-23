#pragma once

#include <memory>
#include <limits>

#include <unordered_map>
#include <unordered_set>
#include <google/sparse_hash_map>
#include <google/sparse_hash_set>
#include <google/dense_hash_map>
#include <google/dense_hash_set>

#include <Core/EigenTypedef.h>

#include "HashKey.h"

/**
 * Typedefs for std hash types.
 * HashType: 1 google sparse
 *           2 google dense
 *           else std hash
 */
template<int DIM, int HashType>
struct HashMapTrait {
    //typedef VectorHashKey<int, DIM> HashKey;
    typedef Vector3IHashKey HashKey;

    struct HashMapFunc {
        int operator()(const HashKey& key) const {
            return key.hash();
        }
    };

    typedef std::unordered_set<int> HashItem;
    typedef std::unordered_map<HashKey, HashItem, HashMapFunc> HashMap;
    typedef std::shared_ptr<HashMap> Ptr;

    static Ptr create_map() { return Ptr(new HashMap()); }
    static HashItem get_default_item() { return HashItem(); }
};

/**
 * Typedefs for google sparse hash.
 */
template<int DIM>
struct HashMapTrait<DIM, 1> {
    //typedef VectorHashKey<int, DIM> HashKey;
    typedef Vector3IHashKey HashKey;

    struct HashMapFunc {
        int operator()(const HashKey& key) const {
            return key.hash();
        }
    };

    typedef google::sparse_hash_set<int> HashItem;
    typedef google::sparse_hash_map<HashKey, HashItem, HashMapFunc> HashMap;
    typedef std::shared_ptr<HashMap> Ptr;


    static Ptr create_map() {
        Ptr hash_map = Ptr(new HashMap());
        hash_map->set_deleted_key(HashKey(get_deleted_val()));
        return hash_map;
    }
    static HashItem get_default_item() {
        HashItem item;
        item.set_deleted_key(get_deleted_val());
        return item;
    }
    static int get_deleted_val() {
        return std::numeric_limits<int>::max();
    }
};

/**
 * Typedefs for google dense hash.
 */
template<int DIM>
struct HashMapTrait<DIM, 2> {
    //typedef VectorHashKey<int, DIM> HashKey;
    typedef Vector3IHashKey HashKey;

    struct HashMapFunc {
        int operator()(const HashKey& key) const {
            return key.hash();
        }
    };

    typedef google::dense_hash_set<int> HashItem;
    typedef google::dense_hash_map<HashKey, HashItem, HashMapFunc> HashMap;
    typedef std::shared_ptr<HashMap> Ptr;

    static const int DELETED_VAL = std::numeric_limits<int>::max();
    static const int EMPTY_VAL = DELETED_VAL-1;

    static Ptr create_map() {
        Ptr hash_map = Ptr(new HashMap());
        hash_map->set_empty_key(HashKey(get_empty_val()));
        hash_map->set_deleted_key(HashKey(get_deleted_val()));
        return hash_map;
    }
    static HashItem get_default_item() {
        HashItem item;
        item.set_empty_key(get_empty_val());
        item.set_deleted_key(get_deleted_val());
        return item;
    }
    static int get_deleted_val() {
        return std::numeric_limits<int>::max();
    }
    static int get_empty_val() {
        return get_deleted_val() - 1;
    }
};

