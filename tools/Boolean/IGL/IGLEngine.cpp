/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "IGLEngine.h"

#include <igl/copyleft/boolean/mesh_boolean.h>

void IGLEngine::compute_union() {
    igl::copyleft::boolean::mesh_boolean(
            m_vertices_1, m_faces_1, 
            m_vertices_2, m_faces_2,
            igl::copyleft::boolean::MESH_BOOLEAN_TYPE_UNION,
            m_vertices, m_faces);
}

void IGLEngine::compute_intersection() {
    igl::copyleft::boolean::mesh_boolean(
            m_vertices_1, m_faces_1, 
            m_vertices_2, m_faces_2,
            igl::copyleft::boolean::MESH_BOOLEAN_TYPE_INTERSECT,
            m_vertices, m_faces);
}

void IGLEngine::compute_difference() {
    igl::copyleft::boolean::mesh_boolean(
            m_vertices_1, m_faces_1, 
            m_vertices_2, m_faces_2,
            igl::copyleft::boolean::MESH_BOOLEAN_TYPE_MINUS,
            m_vertices, m_faces);
}

void IGLEngine::compute_symmetric_difference() {
    igl::copyleft::boolean::mesh_boolean(
            m_vertices_1, m_faces_1, 
            m_vertices_2, m_faces_2,
            igl::copyleft::boolean::MESH_BOOLEAN_TYPE_XOR,
            m_vertices, m_faces);
}
