/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include "HashGrid.h"

namespace PyMesh {

template<typename Trait>
class HashGridImplementation : public HashGrid {
    protected:
        typedef typename Trait::HashKey HashKey;
        typedef typename Trait::HashItem HashItem;
        typedef typename Trait::HashMap HashMap;
        typedef typename Trait::Ptr HashMapPtr;

    public:
        HashGridImplementation(Float cell_size);
        virtual ~HashGridImplementation() {}

    public:
        virtual bool insert(int obj_id, const VectorF& coordinates);
        virtual bool insert_bbox(int obj_id, const MatrixF& shape);
        virtual bool insert_triangle(int obj_id, const MatrixFr& shape);
        virtual bool insert_multiple_triangles(const VectorI& obj_ids, const MatrixFr& shape);
        virtual bool insert_batch(int obj_id, const MatrixFr& points);
        virtual bool insert_multiple(const VectorI& obj_ids, const MatrixFr& points);
        virtual bool remove(int obj_id, const VectorF& coordinate);
        virtual bool occupied(int obj_id, const VectorF& coordinate) const;

        virtual size_t bucket_count() const { return m_hash_map->bucket_count(); }
        virtual size_t size() const { return m_hash_map->size(); }

        virtual VectorI get_items_near_point(const VectorF& coordinate);
        //virtual VectorI get_items_within_radius(const VectorF& coordinate, Float radius);

        virtual MatrixFr get_occupied_cell_centers() const;

    protected:
        HashKey convert_to_key(const VectorF& value) const;
        VectorF convert_to_grid_point(const HashKey& key) const;
        bool insert_key(int obj_id, HashKey& key);

    protected:
        HashMapPtr m_hash_map;
};

}

#include "HashGridImplementation.inl"
