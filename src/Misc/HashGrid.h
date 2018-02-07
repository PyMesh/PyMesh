/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <memory>

#include <Core/EigenTypedef.h>
#include <Core/Exception.h>

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

    public:
        virtual bool insert(int obj_id, const VectorF& coordinates) {
            throw NotImplementedError("HashGrid::insert is not implemented");
        }
        virtual bool insert_bbox(int obj_id, const MatrixF& shape) {
            throw NotImplementedError("HashGrid::insert_bbox is not implemented");
        }
        virtual bool insert_triangle(int obj_id, const MatrixFr& shape) {
            throw NotImplementedError("HashGrid::insert_triangle is not implemented");
        }
        virtual bool insert_multiple_triangles(const VectorI& obj_ids, const MatrixFr& shape) {
            throw NotImplementedError("HashGrid::insert_multiple_triangle is not implemented");
        }
        virtual bool insert_batch(int obj_id, const MatrixFr& points) {
            throw NotImplementedError("HashGrid::insert_batch is not implemented");
        }
        virtual bool insert_multiple(const VectorI& obj_ids, const MatrixFr& points) {
            throw NotImplementedError("hashgrid::insert_multiple is not implemented");
        }
        virtual bool remove(int obj_id, const VectorF& coordinate) {
            throw NotImplementedError("hashgrid::remove is not implemented");
        }
        virtual bool occupied(int obj_id, const VectorF& coordinate) const {
            throw NotImplementedError("hashgrid::occupied is not implemented");
        }

        virtual size_t bucket_count() const {
            throw NotImplementedError("hashgrid::bucket_count is not implemented");
        }
        virtual size_t size() const {
            throw NotImplementedError("hashgrid::size is not implemented");
        }

        virtual VectorI get_items_near_point(const VectorF& coordinate) {
            throw NotImplementedError("hashgrid::get_items_near_point is not implemented");
        }
        //virtual VectorI get_items_within_radius(const VectorF& coordinate, Float radius)=0;
        virtual MatrixFr get_occupied_cell_centers() const {
            throw NotImplementedError("hashgrid::get_occupied_cell_centers is not implemented");
        }

    protected:
        Float m_cell_size;
};

}
