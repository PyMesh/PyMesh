#pragma once

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
        bool is_manifold();

        /**
         * Returns true iff the mesh does not contain any boundary.
         */
        bool is_closed();

        size_t get_num_boundary_edges();

        size_t get_num_boundary_loops();

        int get_genus();

        int get_euler_characteristic();

        size_t get_num_connected_components();

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
