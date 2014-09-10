#pragma once
#include "Subdivision.h"

#include <map>
#include <vector>

#include <Misc/Triplet.h>

class SimpleSubdivision : public Subdivision {
    public:
        virtual ~SimpleSubdivision() {}
        virtual void subdivide(MatrixFr vertices, MatrixIr faces,
                size_t num_iterations);

    private:
        void initialize_face_indices();
        void subdivide_once();
        void subdivide_face(size_t fi);
        VectorI compute_mid_edge_points(const VectorI& face);
        void update_vertices();
        void update_faces();
        void update_face_indices();

    private:
        std::map<Triplet, size_t> m_mid_edge_points_map;
        std::vector<VectorF> m_mid_edge_points;
        std::vector<VectorI> m_subdivided_faces;
        std::vector<size_t>  m_subdivided_face_indices;
};
