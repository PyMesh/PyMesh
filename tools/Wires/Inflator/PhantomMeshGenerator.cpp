/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "PhantomMeshGenerator.h"

#include <cassert>
#include <list>
#include <iostream>

#include <MeshFactory.h>
#include <IO/MeshWriter.h>

#include <Wires/Parameters/ParameterCommon.h>
#include <Wires/Parameters/ThicknessParameters.h>
#include <Wires/Parameters/OffsetParameters.h>
#include <Wires/Tiler/WireTiler.h>

#include "InflatorEngine.h"
#include "SimpleInflator.h"

using namespace PyMesh;

namespace PhantomMeshGeneratorHelper {
    template <typename Derived>
    void batch_assign(Eigen::MatrixBase<Derived>& data,
            const VectorI& indices, Float value) {
        const size_t length = indices.size();
        for (size_t i=0; i<length; i++) {
            data[indices[i]] = value;
        }
    }

    std::string get_indexed_name(const std::string& basename, size_t index) {
        std::stringstream buffer;
        buffer << basename << "_" << index;
        return buffer.str();
    }

    template<typename Derived>
    VectorI get_non_zero_index(const Eigen::MatrixBase<Derived>& data) {
        std::list<size_t> indices;
        const size_t num_entries = data.size();
        for (size_t i=0; i<num_entries; i++) {
            if (data(i, 0) != 0) {
                indices.push_back(i);
            }
        }

        const size_t num_non_zero_entries = indices.size();
        VectorI result(num_non_zero_entries);
        std::copy(indices.begin(), indices.end(), result.data());
        return result;
    }

    void dilate(const MatrixIr& edges, std::vector<bool>& indicator) {
        const size_t num_edges = edges.rows();
        std::vector<bool> ref = indicator;
        for (size_t i=0; i<num_edges; i++) {
            const VectorI& e = edges.row(i);
            if (ref[e[0]] || ref[e[1]]) {
                indicator[e[0]] = true;
                indicator[e[1]] = true;
            }
        }
    }
}
using namespace PhantomMeshGeneratorHelper;


void PhantomMeshGenerator::generate() {
    initialize_wire_network();
    convert_parameters_to_attributes();
    tile();
    trim_irrelavent_edges();
    convert_attributes_to_parameters();
    inflate();
}

std::vector<MatrixFr> PhantomMeshGenerator::get_shape_velocities() const {
    if (m_with_shape_velocities) {
        return m_shape_velocities;
    } else {
        std::stringstream err_msg;
        err_msg << "Shape velocity support is turned off.  "
            << "Please call with_shape_velocity() method to turn it on.";
        throw RuntimeError(err_msg.str());
    }
}

void PhantomMeshGenerator::initialize_wire_network() {
    m_wire_network->center_at_origin();
}

void PhantomMeshGenerator::convert_parameters_to_attributes() {
    assert(m_parameter_manager);
    m_wire_network->clear_attributes();
    m_wire_network->add_attribute("vertex_periodic_index", true);
    m_wire_network->add_attribute("edge_periodic_index", false);

    const ThicknessParameters& thickness_params =
        m_parameter_manager->get_thickness_params();
    const OffsetParameters& offset_params =
        m_parameter_manager->get_offset_params();

    const size_t num_wire_vertices = m_wire_network->get_num_vertices();
    const size_t num_wire_edges = m_wire_network->get_num_edges();

    VectorF empty_vertex_roi_indicator = VectorF::Zero(num_wire_vertices);
    VectorF empty_edge_roi_indicator = VectorF::Zero(num_wire_edges);
    VectorF empty_roi_indicator;
    bool per_vertex_thickness =
        (thickness_params.get_type() == ParameterCommon::VERTEX);
    if (per_vertex_thickness) {
        empty_roi_indicator = empty_vertex_roi_indicator;
    } else {
        empty_roi_indicator = empty_edge_roi_indicator;
    }

    size_t count=0;
    for (const auto& param : thickness_params) {
        VectorI roi = param->get_roi();
        VectorF roi_indicator = empty_roi_indicator;
        batch_assign(roi_indicator, roi, 1.0);
        std::string attr_name = get_indexed_name("thickness_roi", count);
        m_wire_network->add_attribute(attr_name, per_vertex_thickness);
        m_wire_network->set_attribute(attr_name, roi_indicator);
        m_thickness_roi_attr_names.push_back(attr_name);
        count++;
    }

    count=0;
    for (const auto& param : offset_params) {
        VectorI roi = param->get_roi();
        VectorF roi_indicator = empty_vertex_roi_indicator;
        batch_assign(roi_indicator, roi, 1.0);
        std::string roi_attr_name = get_indexed_name("offset_roi", count);
        m_wire_network->add_attribute(roi_attr_name);
        m_wire_network->set_attribute(roi_attr_name, roi_indicator);
        m_offset_roi_attr_names.push_back(roi_attr_name);

        MatrixFr wire_offset_derivative = param->compute_derivative();
        std::string wire_offset_derivative_attr_name =
            get_indexed_name("wire_offset_derivative", count);
        m_wire_network->add_attribute(wire_offset_derivative_attr_name);
        m_wire_network->set_attribute(wire_offset_derivative_attr_name,
                wire_offset_derivative);
        m_offset_derivative_attr_names.push_back(
                wire_offset_derivative_attr_name);
        count++;
    }
}

void PhantomMeshGenerator::tile() {
    const size_t dim = m_wire_network->get_dim();
    VectorF bbox_min = m_wire_network->get_bbox_min();
    VectorF bbox_max = m_wire_network->get_bbox_max();
    bbox_min.array() *= 3;
    bbox_max.array() *= 3;

    WireTiler tiler(m_wire_network);
    tiler.with_parameters(m_parameter_manager);
    m_phantom_wires = tiler.tile_with_guide_bbox(
            bbox_min, bbox_max, VectorI::Ones(dim) * 3);
    m_phantom_wires->center_at_origin();

    assert(m_phantom_wires->has_attribute("thickness"));
    assert(m_phantom_wires->has_attribute("vertex_offset"));
}

void PhantomMeshGenerator::trim_irrelavent_edges() {
    // Relavent edge is edge that have at least 1 end point from the original
    // wire network.
    const Float eps = 1e-3;
    const size_t dim = m_wire_network->get_dim();
    const VectorF bbox_min = m_wire_network->get_bbox_min() - VectorF::Ones(dim)*eps;
    const VectorF bbox_max = m_wire_network->get_bbox_max() + VectorF::Ones(dim)*eps;

    const size_t num_phantom_vertices = m_phantom_wires->get_num_vertices();
    const size_t num_phantom_edges = m_phantom_wires->get_num_edges();
    const MatrixFr& phantom_vertices = m_phantom_wires->get_vertices();
    const MatrixIr& phantom_edges = m_phantom_wires->get_edges();

    std::vector<bool> vertex_relavent_indicator(num_phantom_vertices, false);
    for (size_t i=0; i<num_phantom_vertices; i++) {
        const VectorF& v = phantom_vertices.row(i);
        if ((v.array() > bbox_min.array()).all() &&
                (v.array() < bbox_max.array()).all()) {
            vertex_relavent_indicator[i] = true;
        }
    }

    dilate(phantom_edges, vertex_relavent_indicator);
    dilate(phantom_edges, vertex_relavent_indicator);
    m_phantom_wires->filter_vertices(vertex_relavent_indicator);
}

void PhantomMeshGenerator::convert_attributes_to_parameters() {
    assert(m_parameter_manager);
    const ThicknessParameters& thickness_params =
        m_parameter_manager->get_thickness_params();
    const OffsetParameters& offset_params =
        m_parameter_manager->get_offset_params();

    m_phantom_param_manager =
        ParameterManager::create_empty_manager(m_phantom_wires);
    m_phantom_param_manager->set_thickness_type(
            m_parameter_manager->get_thickness_type());

    size_t count=0;
    for (const auto& param : thickness_params) {
        const std::string& attr_name = m_thickness_roi_attr_names[count];
        const MatrixFr& attr = m_phantom_wires->get_attribute(attr_name);
        VectorI roi = get_non_zero_index(attr);
        m_phantom_param_manager->add_thickness_parameter(
                roi, "", param->get_value());
        count++;
    }

    count=0;
    for (const auto& param : offset_params) {
        const std::string& roi_attr_name = m_offset_roi_attr_names[count];
        const MatrixFr& roi_attr = m_phantom_wires->get_attribute(roi_attr_name);
        VectorI roi = get_non_zero_index(roi_attr);

        const std::string& derivative_name =
            m_offset_derivative_attr_names[count];
        const MatrixFr& derivative =
            m_phantom_wires->get_attribute(derivative_name);
        m_phantom_param_manager->add_custom_offset_parameter(
                roi, "", 0.0, derivative);
        count++;
    }
}

void PhantomMeshGenerator::inflate() {
    assert(m_phantom_param_manager);
    MatrixFr thickness = m_phantom_wires->get_attribute("thickness");
    InflatorEngine::ThicknessType thickness_type = InflatorEngine::PER_VERTEX;
    if (m_phantom_param_manager->get_thickness_type() ==
            ParameterCommon::EDGE) {
        thickness_type = InflatorEngine::PER_EDGE;
    }

    SimpleInflator inflator(m_phantom_wires);
    inflator.set_thickness_type(thickness_type);
    inflator.set_thickness(thickness);
    inflator.set_profile(m_profile);
    inflator.with_rel_geometry_correction(m_rel_geometry_correction);
    inflator.with_abs_geometry_correction(m_abs_geometry_correction);
    inflator.set_geometry_correction_cap(m_geometry_correction_cap);
    inflator.self_intersection_is_fatal();
    inflator.inflate();

    m_vertices = inflator.get_vertices();
    m_faces = inflator.get_faces();
    update_face_sources(inflator.get_face_sources());
    compute_phantom_shape_velocities();

    //VectorF debug_face_sources = m_face_sources_from_ori_wires.cast<Float>();
    //save_mesh("phantom_debug.msh", m_vertices, m_faces, debug_face_sources);
}

void PhantomMeshGenerator::update_face_sources(
        const VectorI& face_sources) {
    const size_t num_faces = face_sources.size();
    m_face_sources_from_ori_wires.resize(num_faces);
    m_face_sources_from_phantom_wires = face_sources;

    const MatrixFr& vertex_periodic_index =
        m_phantom_wires->get_attribute("vertex_periodic_index");
    const MatrixFr& edge_periodic_index =
        m_phantom_wires->get_attribute("edge_periodic_index");

    for (size_t i=0; i<num_faces; i++) {
        int source = face_sources[i];
        if (source < 0) {
            // Edge index
            source = -source - 1;
            m_face_sources_from_ori_wires[i] = -edge_periodic_index(source, 0) - 1;
        } else if (source > 0) {
            // Vertex index
            source  = source - 1;
            m_face_sources_from_ori_wires[i] = vertex_periodic_index(source, 0) + 1;
        }
    }
}

void PhantomMeshGenerator::compute_phantom_shape_velocities() {
    if (!m_with_shape_velocities) return;
    const size_t dim = m_vertices.cols();
    const size_t vertex_per_face = m_faces.cols();
    const size_t vertex_per_voxel = 0;

    MeshFactory factory;
    VectorF flattened_vertices = Eigen::Map<VectorF>(
            m_vertices.data(),
            m_vertices.rows() * m_vertices.cols());
    VectorI flattened_faces = Eigen::Map<VectorI>(
            m_faces.data(),
            m_faces.rows() * m_faces.cols());
    VectorI voxels = VectorI::Zero(0);
    factory.load_data(flattened_vertices, flattened_faces, voxels,
            dim, vertex_per_face, vertex_per_voxel);
    Mesh::Ptr mesh = factory.create_shared();

    VectorF face_sources = m_face_sources_from_phantom_wires.cast<Float>();
    mesh->add_attribute("face_source");
    mesh->set_attribute("face_source", face_sources);

    //VectorF debug_face_sources = m_face_sources_from_ori_wires.cast<Float>();
    //save_mesh("phantom_debug.msh", m_vertices, m_faces, debug_face_sources);

    m_shape_velocities = m_phantom_param_manager->compute_shape_velocity(mesh);
    assert(m_shape_velocities.size() == m_phantom_param_manager->get_num_dofs());
}

void PhantomMeshGenerator::save_mesh(const std::string& filename,
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

