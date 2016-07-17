/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <vector>

#include <Core/EigenTypedef.h>
#include <Misc/TripletMap.h>

namespace PyMesh {

class MeshChecker {
    public:
        MeshChecker(const MatrixFr& vertices, const MatrixIr& faces,
                const MatrixIr& voxels);

    public:
        /**
         * Returns true iff 1 ring of every vertex is topologically a disk.
         * (or half disk for boundary vertices.)
         */
        bool is_vertex_manifold() const;

        /**
         * Return true iff every edge is adjacent to up to 2 faces.
         */
        bool is_edge_manifold() const;

        /**
         * Returns true iff the mesh does not contain any boundary.
         */
        bool is_closed() const;

        /**
         * Returns true iff the mesh contains edges with odd number of adjacent
         * faces.
         */
        bool has_edge_with_odd_adj_faces() const;

        /**
         * A surface is oriented if its normal vector field changes continuously
         * over the surface.  In the discrete setting, it means that all faces
         * are consistantly oriented (e.g. all ccw around outward pointing
         * normal).
         *
         * Note that this method will return true if the surface is oriented but
         * with normals pointing inward.
         */
        bool is_oriented() const;

        /**
         * Returns true iff mesh boundary does not form simple loops.
         */
        bool has_complex_boundary() const { return m_complex_bd; }

        size_t get_num_boundary_edges() const;

        size_t get_num_boundary_loops() const;

        MatrixIr get_boundary_edges() const {
            return m_boundary_edges;
        }

        std::vector<VectorI> get_boundary_loops() const {
            return m_boundary_loops;
        }

        int get_genus() const;

        int get_euler_characteristic() const;

        /**
         * Each component is connected by at least a vertex.
         */
        size_t get_num_connected_components() const;

        /**
         * Each component is connected by at least an edge.
         */
        size_t get_num_connected_surface_components() const;

        /**
         * Each component is connected by at least a face.
         */
        size_t get_num_connected_volume_components() const;

        size_t get_num_isolated_vertices() const;

        size_t get_num_duplicated_faces() const;

        Float compute_signed_volume_from_surface() const;

    private:
        void init_boundary();
        void init_boundary_loops();
        void init_edge_face_adjacency();

    private:
        MatrixFr m_vertices;
        MatrixIr m_faces;
        MatrixIr m_voxels;
        MatrixIr m_boundary_edges;
        TripletMap<size_t> m_edge_face_adjacency;
        std::vector<VectorI> m_boundary_loops;
        bool m_complex_bd;
};

}
