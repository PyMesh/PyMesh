/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include <vector>

#include <Core/EigenTypedef.h>
#include <Mesh.h>
#include <Misc/MultipletMap.h>

namespace PyMesh {

class MeshSeparator {
    public:
        MeshSeparator(const MatrixI& elements);

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

        MatrixI get_component(size_t i) const {
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

        MatrixI flood(size_t seed, VectorI& sources);
        std::vector<size_t> get_adjacent_element(size_t fi);

    private:
        std::vector<MatrixI> m_components;
        std::vector<VectorI> m_sources;
        MatrixI m_elements;
        std::vector<bool> m_visited;

        ConnectivityType m_connectivity_type;

        typedef Multiplet Connector;
        typedef MultipletMap<size_t> ConnectorMap;
        typedef ConnectorMap::ValueType AdjElements;
        ConnectorMap m_connectivity;
};

}
