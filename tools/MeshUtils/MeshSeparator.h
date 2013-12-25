#pragma once
#include <vector>

#include <Core/EigenTypedef.h>
#include <Mesh.h>

#include "TripletMap.h"

class MeshSeparator {
    public:
        MeshSeparator(const MatrixI& faces);

        size_t separate();
        MatrixI get_component(size_t i) {
            return m_components.at(i);
        }

        void clear();

    private:
        void compute_face_connectivity();
        MatrixI flood(size_t seed);
        std::vector<size_t> get_adjacent_faces(size_t fi);

    private:
        std::vector<MatrixI> m_components;
        MatrixI m_faces;
        std::vector<bool> m_visited;

        typedef Triplet Edge;
        typedef TripletMap<size_t> EdgeMapI;
        typedef EdgeMapI::ValueType AdjFaces;
        EdgeMapI m_edge_map;
};

