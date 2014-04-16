#pragma once
#include <unordered_map>
#include <unordered_set>
#include <google/sparse_hash_map>
#include <google/sparse_hash_set>
#include <google/dense_hash_map>
#include <google/dense_hash_set>
#include <list>
#include <memory>
#include <iostream>

#include <Core/EigenTypedef.h>
#include <Misc/HashKey.h>

// HASH_TYPE:
//   0: Standard hash.
//   1: Google sparse hash.
//   2: Google dense hash.
#define HASH_TYPE 2

namespace Zhou {

class HashGrid {
    public:
        typedef std::shared_ptr<HashGrid> Ptr;
        static Ptr create(Float cell_size=1.0);

    private:
        HashGrid(Float cell_size);
        HashGrid(const HashGrid& other) {}

    public:
        typedef Vector3IHashKey HashKey;
        struct HashMapFunc {
            int operator()(const HashKey& key) const {
                return key.hash();
            }
        };

#if HASH_TYPE==0
        typedef __gnu_cxx::hash_set<int> HashItem;
        typedef __gnu_cxx::hash_multimap<HashKey, HashItem, HashMapFunc> HashMap;
#elif HASH_TYPE==1
        typedef google::sparse_hash_set<int> HashItem;
        typedef google::sparse_hash_map<HashKey, HashItem, HashMapFunc> HashMap;
#else
        typedef google::dense_hash_set<int> HashItem;
        typedef google::dense_hash_map<HashKey, HashItem, HashMapFunc> HashMap;
#endif

    public:
        bool insert(int obj_id, const VectorF& coordinates);
        bool insert_bbox(int obj_id, const MatrixF& shape);
        bool insert_batch(int obj_id, const MatrixF& points);
        bool insert_multiple(const VectorI& obj_ids, const MatrixF& points);
        bool remove(int obj_id, const VectorF& coordinates);
        bool occupied(int obj_id, const VectorF& coordinates) const;

        size_t bucket_count() const { return m_hashMap.bucket_count(); }
        size_t size() const { return m_hashMap.size(); }
        const HashItem* get_items(const VectorF& coordinates);
        VectorI get_items_near_point(const VectorF& coordinates);
        void print_hash_efficiency() const;

    private:
        HashKey convert_to_key(const VectorF& value) const;
        HashKey convert_to_key(Float x, Float y, Float z) const;
        bool insert_key(int obj_id, HashKey& key);

    private:
        HashMap m_hashMap;
        Float m_cell_size;
};

}
