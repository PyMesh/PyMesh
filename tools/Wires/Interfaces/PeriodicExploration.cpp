#include "PeriodicExploration.h"

#include <cmath>
#include <iostream>
#include <Core/Exception.h>
#include <MeshFactory.h>
#include <tetgen/TetgenWrapper.h>
#include <tetgen/TetgenException.h>
#include <Wires/Inflator/InflatorEngine.h>
#include <Wires/Inflator/MeshRefiner.h>
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

    m_refine_algorithm = "simple";
    m_refine_order = 0;
}

void PeriodicExploration::with_parameters(
        const std::string& orbit_file, const std::string& modifier_file) {
    m_parameters = ParameterManager::create_from_setting_file(
            m_wire_network, m_default_thickness, orbit_file, modifier_file);
}

void PeriodicExploration::with_all_parameters() {
    m_parameters = ParameterManager::create(
            m_wire_network, m_default_thickness);
}

void PeriodicExploration::with_all_isotropic_parameters() {
    m_parameters = ParameterManager::create_isotropic(
            m_wire_network, m_default_thickness);
}

void PeriodicExploration::with_refinement(
        const std::string& algorithm, size_t order) {
    m_refine_algorithm = algorithm;
    m_refine_order = order;
}

void PeriodicExploration::periodic_inflate() {
    InflatorEngine::Ptr inflator =
        InflatorEngine::create_parametric(m_wire_network, m_parameters);
    inflator->with_shape_velocities();
    if (m_refine_order > 0)
        inflator->with_refinement(m_refine_algorithm, m_refine_order);
    inflator->inflate();

    m_vertices = inflator->get_vertices();
    m_faces = inflator->get_faces();
    m_voxels = MatrixIr::Zero(0, 4);
    m_face_sources = inflator->get_face_sources();
    m_shape_velocities = inflator->get_shape_velocities();
    update_mesh();
}

bool PeriodicExploration::run_tetgen(Float max_tet_vol) {
    const size_t dim = m_vertices.cols();
    const size_t num_vertices = m_vertices.rows();
    TetgenWrapper tetgen(m_vertices, m_faces);
    std::stringstream flags;
    if (max_tet_vol == 0.0) {
        max_tet_vol =
            pow(m_default_thickness * pow(0.5, m_refine_order), dim);
    }
    flags << "pqYQa" << max_tet_vol;
    try {
        tetgen.run(flags.str());
    } catch (TetgenException& e) {
        std::cerr << e.what() << std::endl;
        return false;
    }

    // Important note:
    //
    // The following code is based on rather shaky the observation that tetgen
    // will only append to the existing list of vertices.  Therefore, the face
    // arrays are still valid given the "Y" flag is used.
    MatrixFr vertices = tetgen.get_vertices();
    assert((vertices.block(0, 0, num_vertices, dim).array()==m_vertices.array()).all());
    m_vertices = vertices;
    m_voxels = tetgen.get_voxels();

    const size_t num_vol_vertices = m_vertices.rows();
    for (auto& velocity : m_shape_velocities) {
        velocity.conservativeResize(num_vol_vertices, dim);
        velocity.block(num_vertices, 0,
                num_vol_vertices-num_vertices, dim).setZero();
    }
    update_mesh();

    return true;
}

void PeriodicExploration::update_mesh() {
    const size_t dim = m_vertices.cols();

    const size_t num_vertices = m_vertices.rows();
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

