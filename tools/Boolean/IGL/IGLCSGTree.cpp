#include "IGLCSGTree.h"

#include <igl/MeshBooleanType.h>
#include <Core/Exception.h>

using namespace PyMesh;

void IGLCSGTree::compute_union() {
    IGLCSGTree* tree_1 = dynamic_cast<IGLCSGTree*>(m_tree_1.get());
    IGLCSGTree* tree_2 = dynamic_cast<IGLCSGTree*>(m_tree_2.get());
    m_igl_tree = igl::copyleft::cgal::CSGTree(
            tree_1->m_igl_tree, tree_2->m_igl_tree,
            igl::MESH_BOOLEAN_TYPE_UNION);
}

void IGLCSGTree::compute_intersection() {
    IGLCSGTree* tree_1 = dynamic_cast<IGLCSGTree*>(m_tree_1.get());
    IGLCSGTree* tree_2 = dynamic_cast<IGLCSGTree*>(m_tree_2.get());
    m_igl_tree = igl::copyleft::cgal::CSGTree(
            tree_1->m_igl_tree, tree_2->m_igl_tree,
            igl::MESH_BOOLEAN_TYPE_INTERSECT);
}

void IGLCSGTree::compute_difference() {
    IGLCSGTree* tree_1 = dynamic_cast<IGLCSGTree*>(m_tree_1.get());
    IGLCSGTree* tree_2 = dynamic_cast<IGLCSGTree*>(m_tree_2.get());
    m_igl_tree = igl::copyleft::cgal::CSGTree(
            tree_1->m_igl_tree, tree_2->m_igl_tree,
            igl::MESH_BOOLEAN_TYPE_MINUS);
}

void IGLCSGTree::compute_symmetric_difference() {
    IGLCSGTree* tree_1 = dynamic_cast<IGLCSGTree*>(m_tree_1.get());
    IGLCSGTree* tree_2 = dynamic_cast<IGLCSGTree*>(m_tree_2.get());
    m_igl_tree = igl::copyleft::cgal::CSGTree(
            tree_1->m_igl_tree, tree_2->m_igl_tree,
            igl::MESH_BOOLEAN_TYPE_XOR);
}

VectorI IGLCSGTree::get_face_sources() const {
    const auto& J = m_igl_tree.J();
    VectorI face_sources(J.size());
    std::copy(J.data(), J.data() + J.size(), face_sources.data());
    return face_sources;
}

VectorI IGLCSGTree::get_mesh_sources() const {
    auto sources = get_face_sources();
    const auto num_birth_faces = get_birth_face_sizes();
    std::transform(sources.data(), sources.data() + sources.size(),
            sources.data(), [&](int fi) {
            int cumu_count = 0;
            int mesh_count = 0;
            for (size_t s : num_birth_faces) {
                cumu_count += s;
                if (fi < cumu_count) return mesh_count;
                mesh_count++;
            }
            throw RuntimeError("Face does not belong to any leaf mesh.");
            });
    return sources;
}

MatrixFr IGLCSGTree::get_vertices() const {
    return m_igl_tree.cast_V<MatrixFr>();
}

MatrixIr IGLCSGTree::get_faces() const {
    return m_igl_tree.F();
}

size_t IGLCSGTree::get_num_vertices() const {
    const auto& V = m_igl_tree.V();
    return V.rows();
}

size_t IGLCSGTree::get_num_faces() const {
    const auto& F = m_igl_tree.F();
    return F.rows();
}

std::vector<size_t> IGLCSGTree::get_birth_face_sizes() const {
    if (m_tree_1 && m_tree_2) {
        IGLCSGTree* tree_1 = dynamic_cast<IGLCSGTree*>(m_tree_1.get());
        IGLCSGTree* tree_2 = dynamic_cast<IGLCSGTree*>(m_tree_2.get());
        auto r1 = tree_1->get_birth_face_sizes();
        auto r2 = tree_2->get_birth_face_sizes();
        std::move(r2.begin(), r2.end(), std::back_inserter(r1));
        return r1;
    } else {
        return {m_igl_tree.number_of_birth_faces()};
    }
}

