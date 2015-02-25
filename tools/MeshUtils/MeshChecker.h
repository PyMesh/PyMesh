#pragma once

#include <iostream>

#include <Core/EigenTypedef.h>
#include "TripletMap.h"

class MeshChecker {
    public:
        MeshChecker(const MatrixFr& vertices, const MatrixIr& faces);

    public:
        /**
         * Returns true iff 1 ring of every vertex is topologically a disk.
         * (or half disk for boundary vertices.)
         */
        bool is_manifold() const;

        /**
         * Returns true iff the mesh does not contain any boundary.
         */
        bool is_closed() const;

        size_t get_num_boundary_edges() const;

        size_t get_num_boundary_loops() const;

        int get_genus() const;

        int get_euler_characteristic() const;

        size_t get_num_connected_components() const;

        size_t get_num_isolated_vertices() const;

        size_t get_num_duplicated_faces() const;

    private:
        void init_boundary();
        void init_boundary_loops();
        void init_edge_face_adjacency();

    private:
        MatrixFr m_vertices;
        MatrixIr m_faces;
        MatrixIr m_boundary_edges;
        TripletMap<size_t> m_edge_face_adjacency;
        size_t m_num_bd_loops;
};
