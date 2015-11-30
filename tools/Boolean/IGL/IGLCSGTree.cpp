#include "IGLCSGTree.h"

#include <igl/copyleft/boolean/MeshBooleanType.h>

void IGLCSGTree::compute_union() {
    IGLCSGTree* tree_1 = dynamic_cast<IGLCSGTree*>(m_tree_1.get());
    IGLCSGTree* tree_2 = dynamic_cast<IGLCSGTree*>(m_tree_2.get());
    m_igl_tree = igl::copyleft::boolean::CSGTree(
            tree_1->m_igl_tree, tree_2->m_igl_tree,
            igl::copyleft::boolean::MESH_BOOLEAN_TYPE_UNION);
}

void IGLCSGTree::compute_intersection() {
    IGLCSGTree* tree_1 = dynamic_cast<IGLCSGTree*>(m_tree_1.get());
    IGLCSGTree* tree_2 = dynamic_cast<IGLCSGTree*>(m_tree_2.get());
    m_igl_tree = igl::copyleft::boolean::CSGTree(
            tree_1->m_igl_tree, tree_2->m_igl_tree,
            igl::copyleft::boolean::MESH_BOOLEAN_TYPE_INTERSECT);
}

void IGLCSGTree::compute_difference() {
    IGLCSGTree* tree_1 = dynamic_cast<IGLCSGTree*>(m_tree_1.get());
    IGLCSGTree* tree_2 = dynamic_cast<IGLCSGTree*>(m_tree_2.get());
    m_igl_tree = igl::copyleft::boolean::CSGTree(
            tree_1->m_igl_tree, tree_2->m_igl_tree,
            igl::copyleft::boolean::MESH_BOOLEAN_TYPE_MINUS);
}

void IGLCSGTree::compute_symmetric_difference() {
    IGLCSGTree* tree_1 = dynamic_cast<IGLCSGTree*>(m_tree_1.get());
    IGLCSGTree* tree_2 = dynamic_cast<IGLCSGTree*>(m_tree_2.get());
    m_igl_tree = igl::copyleft::boolean::CSGTree(
            tree_1->m_igl_tree, tree_2->m_igl_tree,
            igl::copyleft::boolean::MESH_BOOLEAN_TYPE_XOR);
}

VectorI IGLCSGTree::get_face_sources() const {
    const auto& J = m_igl_tree.J();
    VectorI face_sources(J.size());
    std::copy(J.data(), J.data() + J.size(), face_sources.data());
    return face_sources;
}

MatrixFr IGLCSGTree::get_vertices() const {
    return m_igl_tree.cast_V<MatrixFr>();
}

MatrixIr IGLCSGTree::get_faces() const {
    return m_igl_tree.F();
}
