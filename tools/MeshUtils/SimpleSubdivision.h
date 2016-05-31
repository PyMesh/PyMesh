/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include "Subdivision.h"

#include <map>
#include <vector>

#include <Misc/Triplet.h>

namespace PyMesh {

class SimpleSubdivision : public Subdivision {
    public:
        virtual ~SimpleSubdivision() {}

        virtual void subdivide(MatrixFr vertices, MatrixIr faces,
                size_t num_iterations);

        virtual const std::vector<ZSparseMatrix>& get_subdivision_matrices() const {
            return m_subdivision_matrices;
        }

    protected:
        void initialize_face_indices();
        void subdivide_once();
        void compute_subdivided_vertices();
        void extract_sub_faces();
        void extract_edges();
        void register_edges(const VectorI& face, size_t base_index);
        Vector3I get_edge_indices(const VectorI& face);

    protected:
        std::map<Triplet, size_t> m_edge_index_map;
        std::vector<ZSparseMatrix> m_subdivision_matrices;
};

}
