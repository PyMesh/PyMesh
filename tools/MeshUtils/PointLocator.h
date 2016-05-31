/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <vector>
#include <Core/EigenTypedef.h>
#include <Misc/HashGrid.h>
#include <Mesh.h>

namespace PyMesh {

class PointLocator {
    public:
        PointLocator(Mesh::Ptr mesh);

        /**
         * Determine the voxel enclosing each point.  Compute barycentric
         * coordinates as a byproduct.
         */
        void locate(const MatrixFr& points);

        VectorI get_enclosing_voxels() const {
            return m_voxel_idx;
        }

        MatrixFr get_barycentric_coords() const {
            return m_barycentric_coords;
        }

        void clear();

    private:
        void init_elements();
        void init_barycentric_solvers();
        void init_hash_grid();

        Float compute_cell_size() const;
        VectorF compute_barycentric_coord(const VectorF& v, size_t elem_idx);

    private:
        Mesh::Ptr m_mesh;

        VectorI m_elements;
        size_t m_vertex_per_element;

        MatrixF m_barycentric_solvers;
        MatrixF m_last_vertices;

        HashGrid::Ptr m_grid;

        VectorI m_voxel_idx;
        MatrixFr m_barycentric_coords;
};

}
