/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "SymmetryChecker.h"

#include <Math/MatrixUtils.h>

#include "SymmetryOperators.h"

using namespace PyMesh;

SymmetryChecker::SymmetryChecker(WireNetwork::Ptr wire_network) :
    m_wire_network(wire_network) {
        update();
}

bool SymmetryChecker::has_cubic_symmetry() const {
    const size_t dim = m_wire_network->get_dim();
    const size_t num_vertices = m_wire_network->get_num_vertices();
    const VectorF bbox_center = m_wire_network->center();
    auto cubic_symm_ops = dim == 2 ?
        SymmetryOperators::generate_cubic_symmetry_operators<2>(bbox_center) :
        SymmetryOperators::generate_cubic_symmetry_operators<3>(bbox_center);

    const auto& vertices = m_wire_network->get_vertices();
    for (size_t i=0; i<num_vertices; i++) {
        const auto& v = vertices.row(i);
        for (const auto& f : cubic_symm_ops) {
            const auto& mapped_v = f(v);
            auto images = m_grid->get_items_near_point(mapped_v);
            if (images.size() == 0) return false;
        }
    }
    return true;
}

void SymmetryChecker::update() {
    const size_t num_vertices = m_wire_network->get_num_vertices();
    m_grid = HashGrid::create(1e-12, m_wire_network->get_dim());
    VectorI vertex_indices = MatrixUtils::range<VectorI>(num_vertices);
    m_grid->insert_multiple(vertex_indices, m_wire_network->get_vertices());
}
