#include "PeriodicExploration.h"

#include <iostream>
#include <Core/Exception.h>
#include <MeshFactory.h>
#include <Wires/Inflator/InflatorEngine.h>
#include <Wires/Parameters/ParameterCommon.h>

PeriodicExploration::PeriodicExploration(const std::string& wire_file,
        Float cell_size,
        Float default_thickness) {
    m_default_thickness = default_thickness;
    m_wire_network = WireNetwork::create(wire_file);

    VectorF half_bbox_size = VectorF::Ones(m_wire_network->get_dim())
        * 0.5 * cell_size;
    m_wire_network->scale_fit(
            -half_bbox_size, half_bbox_size);
    m_parameters = ParameterManager::create_empty_manager(
            m_wire_network, m_default_thickness);
}

void PeriodicExploration::with_parameters(
        const std::string& orbit_file, const std::string& modifier_file) {
    m_parameters = ParameterManager::create_from_setting_file(
            m_wire_network, m_default_thickness, orbit_file, modifier_file);
}

void PeriodicExploration::periodic_inflate() {
    ParameterCommon::Variables vars;
    VectorF thickness = m_parameters->evaluate_thickness(vars);
    MatrixFr offset = m_parameters->evaluate_offset(vars);

    MatrixFr ori_vertices = m_wire_network->get_vertices();
    m_wire_network->set_vertices(ori_vertices + offset);

    InflatorEngine::Ptr inflator = InflatorEngine::create("periodic", m_wire_network);

    if (m_parameters->get_thickness_type() == ParameterCommon::VERTEX) {
        inflator->set_thickness_type(InflatorEngine::PER_VERTEX);
    } else {
        inflator->set_thickness_type(InflatorEngine::PER_EDGE);
    }
    inflator->set_thickness(thickness);
    inflator->inflate();

    m_vertices = inflator->get_vertices();
    m_faces = inflator->get_faces();
    m_face_sources = inflator->get_face_sources();

    update_mesh();
    compute_shape_velocity();

    m_wire_network->set_vertices(ori_vertices);
}

void PeriodicExploration::compute_shape_velocity() {
    m_shape_velocity = m_parameters->compute_shape_velocity(m_mesh);
}

void PeriodicExploration::update_mesh() {
    const size_t num_vertices = m_vertices.rows();
    const size_t dim = m_vertices.cols();
    const size_t num_faces = m_faces.rows();
    const size_t vertex_per_face = m_faces.cols();
    const size_t num_voxels = m_voxels.rows();
    const size_t vertex_per_voxel = m_voxels.cols();

    VectorF flattened_vertices = Eigen::Map<VectorF>(m_vertices.data(),
            num_vertices * dim);
    VectorI flattened_faces = Eigen::Map<VectorI>(m_faces.data(), num_faces *
            vertex_per_face);
    VectorI flattened_voxels = Eigen::Map<VectorI>(m_voxels.data(), num_voxels *
            vertex_per_voxel);
    VectorF face_sources = m_face_sources.cast<Float>();

    MeshFactory factory;
    factory.load_data(
            flattened_vertices, flattened_faces, flattened_voxels,
            dim, vertex_per_face, vertex_per_voxel);
    m_mesh = factory.create_shared();

    m_mesh->add_attribute("face_source");
    m_mesh->set_attribute("face_source", face_sources);
}

