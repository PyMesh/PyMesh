/* This file is part of PyMesh. Copyright (c) 2017 by Qingnan Zhou */
#pragma once
#ifdef WITH_IGL
#include <memory>
#include <iostream>

#include <Core/EigenTypedef.h>
#include <Mesh.h>

namespace PyMesh {

class HarmonicSolver {
    public:
        using Ptr = std::shared_ptr<HarmonicSolver>;
        static Ptr create(const Mesh::Ptr mesh);
        static Ptr create_raw(const MatrixFr& nodes, const MatrixIr& elements);

    public:
        HarmonicSolver(const MatrixFr& nodes, const MatrixIr& elements):
            m_order(1), m_nodes(nodes), m_elements(elements) {}

        void set_nodes(const MatrixFr& nodes) {
            m_nodes = nodes;
        }
        const MatrixFr& get_nodes() const {
            return m_nodes;
        }

        void set_elements(const MatrixIr& elements) {
            m_elements = elements;
        }
        const MatrixIr& get_elements() const {
            return m_elements;
        }

        void set_order(size_t order) { m_order = order; }
        size_t get_order() const { return m_order; }

        void set_boundary_indices(const VectorI& bd_indices) {
            m_bd_indices = bd_indices;
        }
        const VectorI& get_boundary_indices() const {
            return m_bd_indices;
        }

        void set_boundary_values(const MatrixFr& bd_values) {
            m_bd_values = bd_values;
        }
        const MatrixFr& get_boundary_values() const {
            return m_bd_values;
        }

        void pre_process();
        void solve();

        const MatrixFr& get_solution() const {
            return m_solution;
        }

    private:
        size_t m_order;
        MatrixFr m_nodes;
        MatrixIr m_elements;
        VectorI m_bd_indices;
        MatrixFr m_bd_values;
        MatrixFr m_solution;
};

}
#endif
