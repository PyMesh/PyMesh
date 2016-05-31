/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "PeriodicInflator.h"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <MeshFactory.h>
#include <CGAL/AABBTree.h>
#include <Math/ZSparseMatrix.h>
#include <Wires/Parameters/ParameterCommon.h>

#include "SimpleInflator.h"
#include "PhantomMeshGenerator.h"

using namespace PyMesh;

namespace PeriodicInflatorHelper {
    Vector3F compute_barycentric_coord(
            const VectorF& p,
            const VectorF& a,
            const VectorF& b,
            const VectorF& c) {
        VectorF v0 = b-a;
        VectorF v1 = c-a;
        VectorF v2 = p-a;
        Float d00 = v0.dot(v0);
        Float d01 = v0.dot(v1);
        Float d11 = v1.dot(v1);
        Float d20 = v2.dot(v0);
        Float d21 = v2.dot(v1);
        Float denom = d00 * d11 - d01 * d01;
        Float v = (d11 * d20 - d01 * d21) / denom;
        Float w = (d00 * d21 - d01 * d20) / denom;
        Float u = 1.0 - v - w;
        return Vector3F(u, v, w);
    }
}
using namespace PeriodicInflatorHelper;

void PeriodicInflator::inflate() {
    generate_phantom_mesh();
    refine_phantom_mesh();
    initialize_AABB_tree();
    clip_to_center_cell();
    update_shape_velocities();
}

void PeriodicInflator::set_uniform_thickness(Float thickness) {
    m_parameter_manager->set_default_thickness(thickness);
}

void PeriodicInflator::set_thickness(const VectorF& thickness) {
    const size_t thickness_size = thickness.size();
    auto& thickness_params = m_parameter_manager->get_thickness_params();
    for (auto& param : thickness_params) {
        VectorI roi = param->get_roi();
        assert(roi.size() > 0);
        assert(roi.maxCoeff() < thickness_size);
        assert(roi.minCoeff() >= 0);
        Float val = thickness[roi[0]];

        const size_t roi_size = roi.size();
        for (size_t i=1; i<roi_size; i++) {
            if (thickness[roi[i]] != val) {
                throw RuntimeError("Non-uniform thickness for a single orbit");
            }
        }
        param->set_value(val);
    }
}

void PeriodicInflator::generate_phantom_mesh() {
    PhantomMeshGenerator generator(
            m_wire_network, m_parameter_manager, m_profile);
    if (m_with_shape_velocities)
        generator.with_shape_velocities();
    generator.with_rel_geometry_correction(m_rel_correction);
    generator.with_abs_geometry_correction(m_abs_correction);
    generator.set_geometry_correction_cap(m_correction_cap);
    generator.generate();
    m_phantom_vertices = generator.get_vertices();
    m_phantom_faces = generator.get_faces();
    m_phantom_face_sources = generator.get_face_sources();
    if (m_with_shape_velocities)
        m_shape_velocities = generator.get_shape_velocities();
}

void PeriodicInflator::initialize_AABB_tree() {
    m_tree = std::make_shared<AABBTree>(m_phantom_vertices, m_phantom_faces);
}

void PeriodicInflator::set_parameter(ParameterManager::Ptr manager) {
    m_parameter_manager = manager;
    if (m_parameter_manager->get_thickness_type() ==
            ParameterCommon::VERTEX) {
        set_thickness_type(PER_VERTEX);
    } else if (m_parameter_manager->get_thickness_type() ==
            ParameterCommon::EDGE) {
        set_thickness_type(PER_EDGE);
    } else {
        throw NotImplementedError("Unknow thickness type!");
    }
}

void PeriodicInflator::refine_phantom_mesh() {
    if (!m_refiner) return;
    m_refiner->subdivide(m_phantom_vertices, m_phantom_faces, m_subdiv_order);
    m_phantom_vertices = m_refiner->get_vertices();
    m_phantom_faces = m_refiner->get_faces();

    VectorI face_indices = m_refiner->get_face_indices();
    const size_t num_faces = m_phantom_faces.rows();
    VectorI face_sources(num_faces);
    for (size_t i=0; i<num_faces; i++) {
        face_sources[i] = m_phantom_face_sources[face_indices[i]];
    }
    m_phantom_face_sources = face_sources;

    if (m_with_shape_velocities) {
        const std::vector<ZSparseMatrix>& subdiv_matrices =
            m_refiner->get_subdivision_matrices();
        for (auto& shape_velocity : m_shape_velocities) {
            for (const auto& mat : subdiv_matrices) {
                shape_velocity = mat * shape_velocity;
            }
        }
    }

    assert(m_phantom_faces.rows() == m_phantom_face_sources.size());
    //save_mesh("phantom.msh", m_phantom_vertices, m_phantom_faces,
    //        m_phantom_face_sources.cast<Float>());
}

void PeriodicInflator::get_center_cell_bbox(
        VectorF& bbox_min, VectorF& bbox_max) {
    bbox_min = m_wire_network->get_bbox_min();
    bbox_max = m_wire_network->get_bbox_max();
}

void PeriodicInflator::update_shape_velocities() {
    if (!m_with_shape_velocities) return;

    const size_t num_vertices = m_vertices.rows();
    const size_t num_phantom_vertices = m_phantom_vertices.rows();
    VectorF squared_dists;
    VectorI closest_face_indices;
    MatrixFr closest_pts;
    m_tree->look_up_with_closest_points(
            m_vertices, squared_dists, closest_face_indices, closest_pts);

    typedef Eigen::Triplet<Float> T;
    std::vector<T> entries;
    assert(squared_dists.size() == num_vertices);
    for (size_t i=0; i<num_vertices; i++) {
        Float dist = squared_dists[i];
        if (dist > 1e-6) continue;
        size_t face_idx = closest_face_indices[i];
        const VectorI& f = m_phantom_faces.row(face_idx);
        Vector3F weight = compute_barycentric_coord(
                closest_pts.row(i),
                m_phantom_vertices.row(f[0]),
                m_phantom_vertices.row(f[1]),
                m_phantom_vertices.row(f[2]));

        entries.push_back(T(i, f[0], weight[0]));
        entries.push_back(T(i, f[1], weight[1]));
        entries.push_back(T(i, f[2], weight[2]));
    }

    ZSparseMatrix transform(num_vertices, num_phantom_vertices);
    transform.setFromTriplets(entries.begin(), entries.end());

    for (auto& shape_velocity :m_shape_velocities) {
        shape_velocity = transform * shape_velocity;
    }
}

