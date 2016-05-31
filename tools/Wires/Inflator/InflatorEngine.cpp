/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "InflatorEngine.h"

#include <sstream>
#include <iostream>

#include <Core/Exception.h>
#include <IO/MeshWriter.h>
#include <Math/MatrixUtils.h>
#include <MeshUtils/IsolatedVertexRemoval.h>
#include <MeshUtils/DuplicatedVertexRemoval.h>
#include <MeshUtils/ShortEdgeRemoval.h>
#include <MeshUtils/Subdivision.h>
#include <MeshFactory.h>

#include <Wires/Misc/MeshCleaner.h>

#include "SimpleInflator.h"
#include "PeriodicInflator2D.h"
#include "PeriodicInflator3D.h"
#include "IsotropicPeriodicInflator.h"

using namespace PyMesh;

InflatorEngine::Ptr InflatorEngine::create(const std::string& type,
        WireNetwork::Ptr network) {
    const size_t dim = network->get_dim();
    if (type == "simple") {
        return std::make_shared<SimpleInflator>(network);
    } else if (type == "periodic") {
        if (dim == 2) {
            return std::make_shared<PeriodicInflator2D>(network);
        } else if (dim == 3) {
            return std::make_shared<PeriodicInflator3D>(network);
        } else {
            std::stringstream err_msg;
            err_msg << "Unsupported dim: " << dim;
            throw NotImplementedError(err_msg.str());
        }
    } else if (type == "reflective") {
        if (dim == 3) {
            return std::make_shared<IsotropicPeriodicInflator>(network);
        } else {
            throw NotImplementedError(
                    "Isotropic periodic inflator only support 3D mesh");
        }
    } else {
        std::stringstream err_msg;
        err_msg << "Unsupported inflator type: " << type;
        throw NotImplementedError(err_msg.str());
    }
}

InflatorEngine::Ptr InflatorEngine::create_parametric(WireNetwork::Ptr network,
        ParameterManager::Ptr manager) {
    const size_t dim = network->get_dim();
    if (dim == 2) {
        std::shared_ptr<PeriodicInflator2D> ptr =
            std::make_shared<PeriodicInflator2D>(network);
        ptr->set_parameter(manager);
        return ptr;
    } else if (dim == 3) {
        std::shared_ptr<PeriodicInflator3D> ptr =
            std::make_shared<PeriodicInflator3D>(network);
        ptr->set_parameter(manager);
        return ptr;
    } else {
        std::stringstream err_msg;
        err_msg << "Unsupported dim: " << dim;
        throw NotImplementedError(err_msg.str());
    }
}

InflatorEngine::Ptr InflatorEngine::create_isotropic_parametric(
        WireNetwork::Ptr network, ParameterManager::Ptr manager) {
    const size_t dim = network->get_dim();
    if (dim == 3) {
        auto ptr = std::make_shared<IsotropicPeriodicInflator>(network);
        ptr->set_parameter(manager);
        return ptr;
    } else {
            throw NotImplementedError(
                    "Isotropic periodic inflator only support 3D mesh");
    }
}

InflatorEngine::InflatorEngine(WireNetwork::Ptr wire_network) :
    m_wire_network(wire_network),
    m_thickness_type(PER_VERTEX),
    m_subdiv_order(0) {
        const size_t dim = m_wire_network->get_dim();
        if (dim == 3) {
            m_profile = WireProfile::create("square");
        } else if (dim == 2) {
            m_profile = WireProfile::create_2D();
        } else {
            std::stringstream err_msg;
            err_msg << "Unsupported dim: " << dim;
            throw NotImplementedError(err_msg.str());
        }
        m_rel_correction = VectorF::Zero(dim);
        m_abs_correction = VectorF::Zero(dim);
        m_correction_cap = 0.3;
        m_spread_const = 0.0;
        m_self_intersection_is_fatal = false;
    }

void InflatorEngine::with_shape_velocities() {
    throw NotImplementedError(
            "Shape velocity computation is not supported for this type of inflator");
}

void InflatorEngine::set_uniform_thickness(Float thickness) {
    if (m_thickness_type == PER_VERTEX) {
        set_thickness(VectorF::Ones(m_wire_network->get_num_vertices())
                * thickness);
    } else {
        set_thickness(VectorF::Ones(m_wire_network->get_num_edges())
                * thickness);
    }
}

void InflatorEngine::set_thickness(const VectorF& thickness) {
    m_thickness = thickness;
    check_thickness();
}

void InflatorEngine::with_refinement(
        const std::string& algorithm, size_t order) {
    m_refiner = Subdivision::create(algorithm);
    m_subdiv_order = order;
}

void InflatorEngine::with_geometry_correction_lookup(
        const std::string& csv_file) {
    m_profile->with_geometry_correction_lookup(csv_file);
}

void InflatorEngine::with_rel_geometry_correction(const VectorF& correction) {
    if (correction.size() != m_wire_network->get_dim()) {
        std::stringstream err_msg;
        err_msg << "Relative geometry offset dimension mismatch.  Expect "
            << m_wire_network->get_dim() << " but get "
            << correction.size() << " instead.";
        throw RuntimeError(err_msg.str());
    }
    m_rel_correction = correction;
}

void InflatorEngine::with_abs_geometry_correction(const VectorF& correction) {
    if (correction.size() != m_wire_network->get_dim()) {
        std::stringstream err_msg;
        err_msg << "Absolute geometry offset dimension mismatch.  Expect "
            << m_wire_network->get_dim() << " but get "
            << correction.size() << " instead.";
        throw RuntimeError(err_msg.str());
    }
    m_abs_correction = correction;
}

void InflatorEngine::clean_up() {
    remove_isolated_vertices();
    MeshCleaner cleaner;
    cleaner.remove_duplicated_vertices(m_vertices, m_faces, 1e-3);
    remove_short_edges();
    remove_isolated_vertices();
}

void InflatorEngine::remove_isolated_vertices() {
    IsolatedVertexRemoval remover(m_vertices, m_faces);
    remover.run();
    m_vertices = remover.get_vertices();
    m_faces = remover.get_faces();
}

void InflatorEngine::remove_duplicated_vertices() {
    DuplicatedVertexRemoval remover(m_vertices, m_faces);
    remover.run(1e-3);
    m_vertices = remover.get_vertices();
    m_faces = remover.get_faces();
}

void InflatorEngine::remove_short_edges() {
    ShortEdgeRemoval remover(m_vertices, m_faces);
    remover.run(1e-3);

    m_vertices = remover.get_vertices();
    m_faces = remover.get_faces();

    const size_t num_faces = m_faces.rows();
    VectorI face_indices = remover.get_face_indices();
    assert(face_indices.size() == num_faces);
    VectorI face_sources(num_faces);
    for (size_t i=0; i<num_faces; i++) {
        face_sources[i] = m_face_sources(face_indices[i]);
    }
    m_face_sources = face_sources;
}

void InflatorEngine::save_mesh(const std::string& filename,
        const MatrixFr& vertices, const MatrixIr& faces, VectorF debug) {
    VectorF flattened_vertices(vertices.rows() * vertices.cols());
    std::copy(vertices.data(), vertices.data() + vertices.rows() *
            vertices.cols(), flattened_vertices.data());
    VectorI flattened_faces(faces.rows() * faces.cols());
    std::copy(faces.data(), faces.data() + faces.rows() * faces.cols(),
            flattened_faces.data());
    VectorI voxels = VectorI::Zero(0);

    Mesh::Ptr mesh = MeshFactory().load_data(
            flattened_vertices, flattened_faces, voxels,
            vertices.cols(), faces.cols(), 0).create_shared();
    mesh->add_attribute("debug");
    mesh->set_attribute("debug", debug);

    MeshWriter::Ptr writer = MeshWriter::create(filename);
    writer->with_attribute("debug");
    writer->write_mesh(*mesh);
}

void InflatorEngine::save_mesh(const std::string& filename,
        const MatrixFr& vertices, const MatrixIr& faces) {
    auto flattened_vertices = MatrixUtils::flatten<VectorF>(vertices);
    auto flattened_faces = MatrixUtils::flatten<VectorI>(faces);
    VectorI voxels = VectorI::Zero(0);

    MeshWriter::Ptr writer = MeshWriter::create(filename);
    writer->write(flattened_vertices, flattened_faces, voxels,
            vertices.cols(), faces.cols(), 0);
}

void InflatorEngine::check_thickness() const {
    size_t size;
    if (m_thickness_type == PER_VERTEX) {
        size = m_wire_network->get_num_vertices();
    } else if (m_thickness_type == PER_EDGE) {
        size = m_wire_network->get_num_edges();
    } else {
        throw RuntimeError("Invalid thickness type!");
    }

    if (m_thickness.size() != size) {
        std::stringstream err_msg;
        err_msg << "Invalid thickness size: " << m_thickness.size() << std::endl;
        err_msg << "Expect " << size << " thickness entries.";
        throw RuntimeError(err_msg.str());
    }

    Float min_thickness = m_thickness.minCoeff();
    if (min_thickness < 1e-2) {
        std::stringstream err_msg;
        err_msg << "Very thin thickness detected: " << min_thickness <<
            std::endl;
        err_msg << "PyWires assumes thickness have unit mm" << std::endl;
        err_msg << "Thickness of such small value is typically not printible."
            << std::endl;
        throw RuntimeError(err_msg.str());
    }
}

