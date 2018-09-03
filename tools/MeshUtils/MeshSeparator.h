/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include <vector>

#include <Core/EigenTypedef.h>
#include <Mesh.h>
#include <Misc/MultipletMap.h>

namespace PyMesh {

class MeshSeparator {
    public:
        MeshSeparator(const MatrixIr& elements);

        enum ConnectivityType {
            VERTEX,
            FACE,
            VOXEL
        };

        /**
         * Connectivity type define what is considered as "connected".
         */
        void set_connectivity_type(ConnectivityType connectivity) {
            m_connectivity_type = connectivity;
        }

        size_t separate();

        MatrixIr get_component(size_t i) const {
            return m_components.at(i);
        }

        VectorI get_sources(size_t i) const {
            return m_sources.at(i);
        }

        void clear();

    private:
        void compute_connectivity();
        void compute_vertex_connectivity();
        void compute_face_connectivity();
        void compute_voxel_connectivity();

        MatrixIr flood(size_t seed, VectorI& sources);
        std::vector<size_t> get_adjacent_element(size_t fi);

    private:
        std::vector<MatrixIr> m_components;
        std::vector<VectorI> m_sources;
        MatrixIr m_elements;
        std::vector<bool> m_visited;

        ConnectivityType m_connectivity_type;

        using VertexConnector = Singleton;
        using FaceConnector = Duplet;
        using TetConnector = Triplet;
        using HexConnector = Quadruplet;
        using AdjElements = MultipletMap<Singleton, size_t>::ValueType;

        MultipletMap<Singleton, size_t> m_vertex_connectivity;
        MultipletMap<Duplet, size_t> m_face_connectivity;
        MultipletMap<Triplet, size_t> m_tet_connectivity;
        MultipletMap<Quadruplet, size_t> m_hex_connectivity;
};

}
