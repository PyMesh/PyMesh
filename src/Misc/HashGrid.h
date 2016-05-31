/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <memory>

#include <Core/EigenTypedef.h>

#ifdef USE_SPARSEHASH
#define DEFAULT_HASH DENSE_HASH
#else
#define DEFAULT_HASH STL_HASH
#endif

namespace PyMesh {

class HashGrid {
    public:
        enum ImplementationType {
            STL_HASH=0,
            SPARSE_HASH=1,
            DENSE_HASH=2
        };
        typedef std::shared_ptr<HashGrid> Ptr;
        static Ptr create(Float cell_size=1.0, size_t dim=3, ImplementationType impl_type=DEFAULT_HASH);

    protected:
        HashGrid(Float cell_size) : m_cell_size(cell_size) {}
        virtual ~HashGrid() {}

    public:
        virtual bool insert(int obj_id, const VectorF& coordinates)=0;
        virtual bool insert_bbox(int obj_id, const MatrixF& shape)=0;
        virtual bool insert_triangle(int obj_id, const MatrixFr& shape)=0;
        virtual bool insert_multiple_triangles(const VectorI& obj_ids, const MatrixFr& shape)=0;
        virtual bool insert_batch(int obj_id, const MatrixFr& points)=0;
        virtual bool insert_multiple(const VectorI& obj_ids, const MatrixFr& points)=0;
        virtual bool remove(int obj_id, const VectorF& coordinate)=0;
        virtual bool occupied(int obj_id, const VectorF& coordinate) const=0;

        virtual size_t bucket_count() const=0;
        virtual size_t size() const=0;

        virtual VectorI get_items_near_point(const VectorF& coordinate)=0;
        //virtual VectorI get_items_within_radius(const VectorF& coordinate, Float radius)=0;
        virtual MatrixFr get_occupied_cell_centers() const=0;

    protected:
        Float m_cell_size;
};

}
