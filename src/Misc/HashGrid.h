#pragma once

#include <memory>

#include <Core/EigenTypedef.h>

class HashGrid {
    public:
        typedef std::shared_ptr<HashGrid> Ptr;
        static Ptr create(Float cell_size=1.0);

    protected:
        HashGrid(Float cell_size) : m_cell_size(cell_size) {}
        virtual ~HashGrid() {}

    public:
        virtual bool insert(int obj_id, const VectorF& coordinates)=0;
        virtual bool insert_bbox(int obj_id, const MatrixF& shape)=0;
        virtual bool insert_batch(int obj_id, const MatrixFr& points)=0;
        virtual bool insert_multiple(const VectorI& obj_ids, const MatrixFr& points)=0;
        virtual bool remove(int obj_id, const VectorF& coordinate)=0;
        virtual bool occupied(int obj_id, const VectorF& coordinate) const=0;

        virtual size_t bucket_count() const=0;
        virtual size_t size() const=0;

        virtual VectorI get_items_near_point(const VectorF& coordinate)=0;
        //virtual VectorI get_items_within_radius(const VectorF& coordinate, Float radius)=0;

    protected:
        Float m_cell_size;
};
