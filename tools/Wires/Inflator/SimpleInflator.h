/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include "InflatorEngine.h"

#include <list>
#include <vector>

namespace PyMesh {

class SimpleInflator : public InflatorEngine {
    public:
        SimpleInflator(WireNetwork::Ptr wire_network)
            : InflatorEngine(wire_network) {}
        virtual ~SimpleInflator() {}

    public:
        virtual void inflate();
        virtual const std::vector<MatrixFr>& get_shape_velocities() const;

    protected:
        void initialize();
        void compute_end_loop_offsets();
        void generate_end_loops();
        void generate_joints();
        void connect_end_loops();
        void finalize();
        void refine();

        VectorF compute_vertex_thickness() const;
        void validate_end_loop_offset() const;
        MatrixFr get_edge_thickness() const;
        void generate_joint(const MatrixFr& pts, const VectorI& source_ids, size_t vertex_index);
        bool belong_to_the_same_loop(
                const VectorI& indices, const VectorI& source_ids) const;

    protected:
        VectorF m_end_loop_offsets;
        std::vector<std::pair<MatrixFr, MatrixFr> > m_end_loops;

        size_t m_num_vertex_accumulated;
        std::list<MatrixFr> m_vertex_list;
        std::list<MatrixIr> m_face_list;
        std::list<int> m_face_source_list;
};

}
