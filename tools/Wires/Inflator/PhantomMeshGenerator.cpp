#include "PhantomMeshGenerator.h"

#include <cassert>
#include <list>

#include <MeshFactory.h>

#include <Wires/Parameters/ParameterCommon.h>
#include <Wires/Parameters/ThicknessParameters.h>
#include <Wires/Parameters/OffsetParameters.h>
#include <Wires/Tiler/WireTiler.h>

#include "InflatorEngine.h"
#include "SimpleInflator.h"

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
}
using namespace PhantomMeshGeneratorHelper;


void PhantomMeshGenerator::generate() {
    initialize_wire_network();
    convert_parameters_to_attributes();
    tile();
    convert_attributes_to_parameters();
    inflate();
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
    if (thickness_params.get_type() == ParameterCommon::VERTEX) {
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
        m_wire_network->add_attribute(attr_name);
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

    ParameterCommon::Variables vars;
    MatrixFr offset = m_parameter_manager->evaluate_offset(vars);
    MatrixFr ori_vertices = m_wire_network->get_vertices();
    m_wire_network->set_vertices(ori_vertices + offset);

    WireTiler tiler(m_wire_network);
    tiler.with_parameters(m_parameter_manager);
    m_phantom_wires = tiler.tile_with_guide_bbox(
            bbox_min, bbox_max, VectorI::Ones(dim) * 3);
    m_phantom_wires->center_at_origin();

    assert(m_phantom_wires->has_attribute("thickness"));
    assert(m_phantom_wires->has_attribute("vertex_offset"));

    m_wire_network->set_vertices(ori_vertices);
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
        m_phantom_param_manager->add_offset_parameter(
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
    inflator.inflate();

    m_vertices = inflator.get_vertices();
    m_faces = inflator.get_faces();
    update_face_sources(inflator.get_face_sources());
    compute_phantom_shape_velocity();
}

void PhantomMeshGenerator::update_face_sources(
        const VectorI& face_sources) {
    const size_t num_faces = face_sources.size();
    m_face_sources.resize(num_faces);

    const MatrixFr& vertex_periodic_index =
        m_phantom_wires->get_attribute("vertex_periodic_index");
    const MatrixFr& edge_periodic_index =
        m_phantom_wires->get_attribute("edge_periodic_index");

    for (size_t i=0; i<num_faces; i++) {
        int source = face_sources[i];
        if (source < 0) {
            // Edge index
            source = -source - 1;
            m_face_sources[i] = -edge_periodic_index(source, 0) - 1;
        } else if (source > 0) {
            // Vertex index
            source  = source - 1;
            m_face_sources[i] = vertex_periodic_index(source, 0) + 1;
        }
    }
}

void PhantomMeshGenerator::compute_phantom_shape_velocity() {
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

    m_shape_velocities = m_phantom_param_manager->compute_shape_velocity(mesh);
}

