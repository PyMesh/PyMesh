/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include "Subdivision.h"

#include <map>
#include <vector>

#include <Math/ZSparseMatrix.h>
#include <Misc/Triplet.h>

namespace PyMesh {

class LoopSubdivision : public Subdivision {
    public:
        virtual ~LoopSubdivision() {}

        virtual void subdivide(MatrixFr vertices, MatrixIr faces,
                size_t num_iterations);

        virtual const std::vector<ZSparseMatrix>& get_subdivision_matrices() const {
            return m_subdivision_matrices;
        }

    protected:
        void subdivide_once();
        void initialize_face_indices();
        void extract_edges();
        void compute_vertex_valance();
        void extract_sub_faces();
        void compute_subdivided_vertices();

        void register_edges(const VectorI& face, size_t base_index);

        Float compute_beta(size_t valance);
        Vector3I get_edge_indices(const VectorI& face);
        std::vector<bool> compute_boundary_vertices() const;

    protected:
        std::map<Triplet, size_t> m_edge_index_map;
        std::map<Triplet, size_t> m_boundary_edge;
        std::vector<size_t> m_vertex_valance;
        std::vector<ZSparseMatrix> m_subdivision_matrices;
};

}
