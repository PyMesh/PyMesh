/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "ParameterManager.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <cassert>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include <Core/Exception.h>

#include "EdgeThicknessParameterDerivative.h"
#include "IsotropicDofExtractor.h"
#include "SymmetryOrbits.h"
#include "VertexThicknessParameterDerivative.h"
#include "VertexOffsetParameter.h"
#include "VertexOffsetParameterDerivative.h"

using namespace PyMesh;
using namespace ParameterCommon;

namespace ParameterManagerHelper {
    typedef boost::property_tree::ptree PTree;

    std::string get_basename(const std::string& filename) {
        size_t pos = filename.find_last_of('.');
        return filename.substr(0, pos);
    }

    Float parse_value(const std::string& data) {
        Float result = 0.0;
        try {
            result = std::stof(data);
        } catch (std::exception& e) {
            // conversion failed.
        }
        return result;
    }

    std::string parse_formula(const std::string& data) {
        size_t begin_pos = data.find('{');
        size_t end_pos = data.find('}');
        if (begin_pos != std::string::npos && end_pos != std::string::npos) {
            assert(end_pos > begin_pos);
            return data.substr(begin_pos+1, end_pos - begin_pos - 1);
        } else {
            return "";
        }
    }

    void parse_offset_parameters(ParameterManager::Ptr manager,
            PTree& config, SymmetryOrbits& orbits) {
        std::string type = config.get<std::string>("type");
        if (type != "vertex_orbit") {
            std::stringstream err_msg;
            err_msg << "Invalid offset type: " << type;
            throw NotImplementedError(err_msg.str());
        }

        PTree& effective_orbits = config.get_child("effective_orbits");
        PTree& offset_percentages = config.get_child("offset_percentages");
        assert(effective_orbits.size() == offset_percentages.size());

        auto orbit_itr = effective_orbits.begin();
        auto offset_itr = offset_percentages.begin();
        while (orbit_itr != effective_orbits.end()) {
            assert(offset_itr != offset_percentages.end());
            size_t orbit_index = orbit_itr->second.get_value<int>();
            const VectorI& roi = orbits.get_vertex_orbit(orbit_index);
            size_t coordinate_idx = 0;
            for (auto coordinate_itr : offset_itr->second) {
                std::string offset =
                    coordinate_itr.second.get_value<std::string>();
                Float val = parse_value(offset);
                std::string formula = parse_formula(offset);
                manager->add_offset_parameter(roi, formula, val, coordinate_idx);
                coordinate_idx++;
            }
            orbit_itr++;
            offset_itr++;
        }
    }

    void parse_thickness_parameters(ParameterManager::Ptr manager,
            PTree& config, SymmetryOrbits& orbits) {
        std::string type = config.get<std::string>("type");
        Float default_thickness = config.get<Float>("default");
        manager->set_default_thickness(default_thickness);
        if (type == "vertex_orbit") {
            manager->set_thickness_type(ParameterCommon::VERTEX);
        } else if (type == "edge_orbit") {
            manager->set_thickness_type(ParameterCommon::EDGE);
        } else {
            std::stringstream err_msg;
            err_msg << "Invalid thickness type: " << type;
            throw NotImplementedError(err_msg.str());
        }

        PTree& effective_orbits = config.get_child("effective_orbits");
        PTree& thicknesses = config.get_child("thickness");
        auto orbit_itr = effective_orbits.begin();
        auto thickness_itr = thicknesses.begin();
        while (orbit_itr != effective_orbits.end()) {
            assert(thickness_itr != thicknesses.end());

            size_t orbit_index = orbit_itr->second.get_value<int>();
            VectorI roi;
            if (type == "vertex_orbit") {
                roi = orbits.get_vertex_orbit(orbit_index);
            } else {
                roi = orbits.get_edge_orbit(orbit_index);
            }

            std::string thickness =
                thickness_itr->second.get_value<std::string>();
            Float val = parse_value(thickness);
            std::string formula = parse_formula(thickness);
            manager->add_thickness_parameter(roi, formula, val);

            orbit_itr++;
            thickness_itr++;
        }
    }

    std::list<VectorI> group_by_label(const MatrixFr& labels) {
        std::map<int, std::vector<size_t> > groups;
        const size_t num_entries = labels.rows();
        for (size_t i=0; i<num_entries; i++) {
            int label = int(std::round(labels(i, 0)));
            auto itr = groups.find(label);
            if (itr == groups.end()) {
                std::vector<size_t> group;
                group.push_back(i);
                groups.emplace(label, group);
            } else {
                itr->second.push_back(i);
            }
        }

        std::list<VectorI> result;
        for (const auto& group : groups) {
            const size_t group_size = group.second.size();
            result.emplace_back(group_size);
            std::copy(group.second.begin(), group.second.end(),
                    result.back().data());
        }
        return result;
    }
}

using namespace ParameterManagerHelper;

ParameterManager::Ptr ParameterManager::create(
        WireNetwork::Ptr wire_network,
        Float default_thickness,
        ParameterManager::TargetType thickness_type) {
    const size_t dim = wire_network->get_dim();
    Ptr manager = create_empty_manager(wire_network, default_thickness);
    if (!wire_network->has_attribute("vertex_symmetry_orbit")) {
        wire_network->add_attribute("vertex_symmetry_orbit");
    }
    if (!wire_network->has_attribute("edge_symmetry_orbit")) {
        wire_network->add_attribute("edge_symmetry_orbit");
    }

    std::list<VectorI> vertex_orbits = group_by_label(
            wire_network->get_attribute("vertex_symmetry_orbit"));
    std::list<VectorI> edge_orbits = group_by_label(
            wire_network->get_attribute("edge_symmetry_orbit"));

    manager->set_thickness_type(thickness_type);
    if (thickness_type == ParameterCommon::VERTEX) {
        for (const auto& roi : vertex_orbits) {
            manager->add_thickness_parameter(roi, "", default_thickness);
        }
    } else {
        for (const auto& roi : edge_orbits) {
            manager->add_thickness_parameter(roi, "", default_thickness);
        }
    }

    manager->set_offset_type(ParameterCommon::VERTEX);
    for (const auto& roi : vertex_orbits) {
        for (size_t i=0; i<dim; i++) {
            manager->add_offset_parameter(roi, "", 0.0, i);
        }
    }
    manager->m_dof_type = ORTHOTROPIC;
    return manager;
}

ParameterManager::Ptr ParameterManager::create_isotropic(
        WireNetwork::Ptr wire_network,
        Float default_thickness,
        ParameterManager::TargetType thickness_type) {
    const size_t dim = wire_network->get_dim();
    Ptr manager = create_empty_manager(wire_network, default_thickness);
    if (!wire_network->has_attribute("vertex_cubic_symmetry_orbit")) {
        wire_network->add_attribute("vertex_cubic_symmetry_orbit");
    }
    if (!wire_network->has_attribute("edge_cubic_symmetry_orbit")) {
        wire_network->add_attribute("edge_cubic_symmetry_orbit");
    }

    std::list<VectorI> vertex_orbits = group_by_label(
            wire_network->get_attribute("vertex_cubic_symmetry_orbit"));
    std::list<VectorI> edge_orbits = group_by_label(
            wire_network->get_attribute("edge_cubic_symmetry_orbit"));

    manager->set_thickness_type(thickness_type);
    if (thickness_type == ParameterCommon::VERTEX) {
        for (const auto& roi : vertex_orbits) {
            manager->add_thickness_parameter(roi, "", default_thickness);
        }
    } else {
        for (const auto& roi : edge_orbits) {
            manager->add_thickness_parameter(roi, "", default_thickness);
        }
    }

    IsotropicDofExtractor extractor(wire_network);
    const MatrixFr& vertices = wire_network->get_vertices();
    manager->set_offset_type(ParameterCommon::VERTEX);
    for (const auto& roi : vertex_orbits) {
        const size_t seed_idx = roi.minCoeff();
        VectorF seed = vertices.row(seed_idx);

        const auto dofs = extractor.extract_dofs(seed);
        for (const auto& dof_dir : dofs) {
            manager->add_isotropic_offset_parameter(
                    roi, "", 0.0, dof_dir);
        }
    }
    manager->m_dof_type = ISOTROPIC;
    return manager;
}

ParameterManager::Ptr ParameterManager::create_empty_manager(
        WireNetwork::Ptr wire_network, Float default_thickness) {
    return std::make_shared<ParameterManager>(wire_network, default_thickness);
}

ParameterManager::Ptr ParameterManager::create_from_setting_file(
        WireNetwork::Ptr wire_network, Float default_thickness,
        const std::string& orbit_file, const std::string& modifier_file) {
    Ptr manager = create_empty_manager(wire_network, default_thickness);
    SymmetryOrbits orbits(orbit_file);

    PTree config;
    read_json(modifier_file, config);

    if (config.find("vertex_offset") != config.not_found()) {
        parse_offset_parameters(manager,
                config.get_child("vertex_offset"), orbits);
    }
    if (config.find("thickness") != config.not_found()) {
        parse_thickness_parameters(manager,
                config.get_child("thickness"), orbits);
    }
    return manager;
}

ParameterManager::Ptr ParameterManager::create_from_dof_file(
        WireNetwork::Ptr wire_network, Float default_thickness,
        const std::string& dof_file) {
    PTree dof_config;
    read_json(dof_file, dof_config);
    std::string dof_type = dof_config.get<std::string>("dof_type");
    std::string thickness_type_str =
        dof_config.get<std::string>("thickness_type");

    PTree& dof_values = dof_config.get_child("dof");
    VectorF dof = VectorF::Zero(dof_values.size());
    size_t count = 0;
    for (const auto& value : dof_values) {
        dof[count] = value.second.get_value<Float>();
        count++;
    }
    assert(count == dof.size());

    TargetType thickness_type = ParameterCommon::VERTEX;
    if (thickness_type_str == "vertex") {
        thickness_type = ParameterCommon::VERTEX;
    } else if (thickness_type_str == "edge") {
        thickness_type = ParameterCommon::EDGE;
    }

    Ptr manager;
    if (dof_type == "isotropic") {
        manager = ParameterManager::create_isotropic(wire_network,
                default_thickness, thickness_type);
    } else if (dof_type == "orthotropic") {
        manager = ParameterManager::create(wire_network,
                default_thickness, thickness_type);
    } else {
        std::stringstream err_msg;
        err_msg << "Unknown dof type: " << dof_type;
        throw NotImplementedError(err_msg.str());
    }

    size_t num_dofs = manager->get_num_dofs();
    if (num_dofs > dof.size()) {
        std::stringstream err_msg;
        err_msg << "Dof mismatch: expect " << num_dofs
            << " dofs, but only received " << dof.size() << " values";
        throw RuntimeError(err_msg.str());
    }
    manager->set_dofs(dof.segment(0, num_dofs));
    return manager;
}

ParameterManager::ParameterManager(
        WireNetwork::Ptr wire_network, Float default_thickness) :
    m_wire_network(wire_network),
    m_thickness_params(wire_network, ParameterCommon::VERTEX, default_thickness),
    m_offset_params(wire_network, ParameterCommon::VERTEX, 0.0),
    m_dof_type(UNKNOWN) { }

size_t ParameterManager::get_num_dofs() const {
    return m_thickness_params.get_num_dofs() + m_offset_params.get_num_dofs();
}

size_t ParameterManager::get_num_thickness_dofs() const {
    return m_thickness_params.get_num_dofs();
}

size_t ParameterManager::get_num_offset_dofs() const {
    return m_offset_params.get_num_dofs();
}

VectorF ParameterManager::get_dofs() const {
    const size_t num_thickness_dofs = m_thickness_params.get_num_dofs();
    const size_t num_offset_dofs = m_offset_params.get_num_dofs();
    VectorF dofs(num_thickness_dofs + num_offset_dofs);
    dofs.segment(0, num_thickness_dofs) = m_thickness_params.get_dofs();
    dofs.segment(num_thickness_dofs, num_offset_dofs) =
        m_offset_params.get_dofs();
    return dofs;
}

void ParameterManager::set_dofs(const VectorF& values) {
    const size_t num_thickness_dofs = m_thickness_params.get_num_dofs();
    const size_t num_offset_dofs = m_offset_params.get_num_dofs();
    if (num_thickness_dofs + num_offset_dofs != values.size()) {
        std::stringstream err_msg;
        err_msg << "Invalid number of dofs!  Expecting " << 
            num_thickness_dofs + num_offset_dofs << " values, got " <<
            values.size() << " values";
        throw RuntimeError(err_msg.str());
    }

    m_thickness_params.set_dofs(values.segment(0, num_thickness_dofs));
    m_offset_params.set_dofs(
            values.segment(num_thickness_dofs, num_offset_dofs));
}

std::vector<MatrixFr> ParameterManager::compute_shape_velocity(Mesh::Ptr mesh) {
    std::vector<MatrixFr> velocity;
    for (auto param : m_thickness_params) {
        ParameterDerivative::Ptr param_derivative;
        if (param->get_type() == PatternParameter::VERTEX_THICKNESS) {
            param_derivative = std::make_shared<
                VertexThicknessParameterDerivative>(mesh, param);
        } else if (param->get_type() == PatternParameter::EDGE_THICKNESS) {
            param_derivative = std::make_shared<
                EdgeThicknessParameterDerivative>(mesh, param);
        } else {
            assert(false);
        }
        velocity.push_back(param_derivative->compute());
    }

    for (auto param : m_offset_params) {
        ParameterDerivative::Ptr param_derivative;
        assert(param->get_type() == PatternParameter::VERTEX_OFFSET);
        param_derivative =
            std::make_shared<VertexOffsetParameterDerivative>(mesh, param);
        velocity.push_back(param_derivative->compute());
    }
    assert(velocity.size() == get_num_dofs());

    return velocity;
}

MatrixFr ParameterManager::compute_wire_gradient(size_t i) const {
    // This is probably not fast, but the number of parameters are typically
    // low.
    const size_t num_thickness_dofs = get_num_thickness_dofs();
    if (i < num_thickness_dofs) {
        size_t count = 0;
        for (const auto& param : m_thickness_params) {
            if (count == i)
                return param->compute_derivative();
            count++;
        }
    } else {
        size_t count = num_thickness_dofs;
        for (const auto& param : m_offset_params) {
            if (count == i)
                return param->compute_derivative();
            count++;
        }

    }

    throw RuntimeError("Parameter index out of bound!");
}

VectorI ParameterManager::get_thickness_dof_map() const {
    size_t domain_size = 0;
    if (get_thickness_type() == ParameterCommon::VERTEX) {
        domain_size = m_wire_network->get_num_vertices();
    } else {
        domain_size = m_wire_network->get_num_edges();
    }
    VectorI dof_map = VectorI::Ones(domain_size) * -1;

    size_t dof_counter = 0;
    for (const auto& param : m_thickness_params) {
        VectorI roi = param->get_roi();
        const size_t roi_size = roi.size();
        for (size_t i=0; i<roi_size; i++) {
            if (dof_map[roi[i]] != -1) {
                throw RuntimeError("Thickness parameters have overlapping ROI");
            }
            dof_map[roi[i]] = dof_counter;
        }
        dof_counter++;
    }
    return dof_map;
}

MatrixIr ParameterManager::get_offset_dof_map() const {
    if (m_dof_type == ISOTROPIC) {
        throw RuntimeError("Offset DOF map does not support isotropic dofs!");
    }

    const size_t num_thickness_dofs = get_num_thickness_dofs();
    const size_t domain_size = m_wire_network->get_num_vertices();
    const size_t dim = m_wire_network->get_dim();
    MatrixIr dof_map = MatrixIr::Ones(domain_size, dim) * -1;

    size_t dof_counter = num_thickness_dofs;
    for (const auto& param : m_offset_params) {
        VectorI roi = param->get_roi();
        const size_t roi_size = roi.size();

        const VertexOffsetParameter* param_ptr =
            dynamic_cast<const VertexOffsetParameter*>(param.get());
        size_t axis = param_ptr->get_axis();

        for (size_t i=0; i<roi_size; i++) {
            dof_map(roi[i], axis) = dof_counter;
        }
        dof_counter++;
    }
    return dof_map;
}

void ParameterManager::save_dofs(const std::string& dof_file) const {
    PTree dof_config;
    switch (m_dof_type) {
        case ISOTROPIC:
            dof_config.put("dof_type", "isotropic");
            break;
        case ORTHOTROPIC:
            dof_config.put("dof_type", "orthotropic");
            break;
        default:
            throw RuntimeError("Cannot save dof with unknown dof type");
    }
    switch (get_thickness_type()) {
        case ParameterCommon::VERTEX:
            dof_config.put("thickness_type", "vertex");
            break;
        case ParameterCommon::EDGE:
            dof_config.put("thickness_type", "edge");
            break;
        default:
            throw RuntimeError("Unknown thickness type.");
    }

    PTree dof_array;
    const size_t num_dofs = get_num_dofs();
    VectorF dofs = get_dofs();
    for (size_t i=0; i<num_dofs; i++) {
        PTree value;
        value.put("", dofs[i]);
        dof_array.push_back(std::make_pair("", value));
    }
    dof_config.add_child("dof", dof_array);
    write_json(dof_file, dof_config);
}

void ParameterManager::load_dofs(const std::string& dof_file) {
    PTree dof_config;
    read_json(dof_file, dof_config);

    std::string dof_type = dof_config.get<std::string>("dof_type");
    if (m_dof_type == UNKNOWN ||
            (m_dof_type == ISOTROPIC && dof_type != "isotropic") ||
            (m_dof_type == ORTHOTROPIC && dof_type != "orthotropic")) {
        std::stringstream err_msg;
        err_msg << "Dof type mismatch: " << dof_type;
        throw RuntimeError(err_msg.str());
    }

    TargetType thickness_type = get_thickness_type();
    std::string thickness_type_str =
        dof_config.get<std::string>("thickness_type");
    if ((thickness_type == ParameterCommon::VERTEX &&
         thickness_type_str != "vertex") ||
        (thickness_type == ParameterCommon::EDGE &&
         thickness_type_str != "edge")) {
        std::stringstream err_msg;
        err_msg << "thickness type mismatch: " << thickness_type_str;
        throw RuntimeError(err_msg.str());
    }

    PTree& dof_values = dof_config.get_child("dof");
    VectorF dof = VectorF::Zero(dof_values.size());
    size_t count = 0;
    for (const auto& value : dof_values) {
        dof[count] = value.second.get_value<Float>();
        count++;
    }
    assert(count == dof.size());
    set_dofs(dof);
}

std::vector<std::string> ParameterManager::get_formulas() const {
    const size_t num_thickness_dofs = get_num_thickness_dofs();
    const size_t num_offset_dofs = get_num_offset_dofs();
    std::vector<std::string> formulas;
    for (const auto& param : m_thickness_params) {
        formulas.emplace_back(param->get_formula());
    }
    for (const auto& param : m_offset_params) {
        formulas.emplace_back(param->get_formula());
    }
    return formulas;
}

VectorF ParameterManager::evaluate_thickness_no_formula() {
    ParameterManager::Variables vars;
    return evaluate_thickness(vars);
}

VectorF ParameterManager::evaluate_thickness(
        const ParameterManager::Variables& vars) {
    return m_thickness_params.evaluate(vars);
}

MatrixFr ParameterManager::evaluate_offset_no_formula() {
    ParameterManager::Variables vars;
    return evaluate_offset(vars);
}

MatrixFr ParameterManager::evaluate_offset(
        const ParameterManager::Variables& vars) {
    return m_offset_params.evaluate(vars);
}

void ParameterManager::set_thickness_type(
        ParameterManager::TargetType type) {
    Float default_thickness = m_thickness_params.get_default();
    WireNetwork::Ptr wires = m_thickness_params.get_wire_network();
    m_thickness_params = ThicknessParameters(
            wires, type, default_thickness);
}

void ParameterManager::add_thickness_parameter(
        const VectorI& roi, const std::string& formula, Float value) {
    m_thickness_params.add(roi, formula, value);
}

void ParameterManager::set_offset_type(
        ParameterManager::TargetType type) {
    Float default_offset = m_offset_params.get_default();
    WireNetwork::Ptr wires = m_offset_params.get_wire_network();
    m_offset_params = OffsetParameters(wires, type, default_offset);
}

void ParameterManager::add_offset_parameter(
        const VectorI& roi, const std::string& formula, Float value,
        size_t axis) {
    m_offset_params.add(roi, formula, value, axis);
}

void ParameterManager::add_custom_offset_parameter(const VectorI& roi,
        const std::string& formula, Float value,
        const MatrixFr& custom_offset) {
    m_offset_params.add(roi, formula, value, custom_offset);
}

void ParameterManager::add_isotropic_offset_parameter(const VectorI& roi,
        const std::string& formula, Float value, const VectorF& dof_dir) {
    m_offset_params.add_isotropic(roi, formula, value, dof_dir);
}

