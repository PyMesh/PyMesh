/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include <functional>
#include <vector>

#include <Core/EigenTypedef.h>
#include <Mesh.h>
#include <Misc/HashGrid.h>

#include "Grid.h"

namespace PyMesh {

template<int DIM>
class VoxelGrid : public Grid<DIM, short>{
    public:
        std::shared_ptr<VoxelGrid<DIM> > Ptr;
        typedef Grid<DIM, short> Parent;
        typedef typename Grid<DIM, short>::Vector_f Vector_f;
        typedef typename Grid<DIM, short>::Vector_i Vector_i;

    public:
        VoxelGrid(Float cell_size);
        virtual ~VoxelGrid() {}

    public:
        void insert_mesh(Mesh::Ptr mesh);
        void create_grid();
        void erode(size_t iterations);
        void dilate(size_t iterations);
        Mesh::Ptr get_voxel_mesh();

    protected:
        typedef Grid<DIM, bool> Mask;

        void insert_triangle_mesh(Mesh::Ptr mesh);
        void insert_quad_mesh(Mesh::Ptr mesh);

        Mask create_mask() const;
        void flood_exterior_cells();
        void flood_from_base_cell(Mask& mask);

    private:
        size_t m_margin;
        HashGrid::Ptr m_hash_grid;
};

}

#include "VoxelGrid.inl"

namespace PyMesh {
typedef VoxelGrid<2> VoxelGrid2D;
typedef VoxelGrid<3> VoxelGrid3D;
}

