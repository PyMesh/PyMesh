#include "ParameterManager.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <cassert>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include <Core/Exception.h>

#include "SymmetryOrbits.h"
#include "EdgeThicknessParameterDerivative.h"
#include "VertexThicknessParameterDerivative.h"
#include "VertexOffsetParameterDerivative.h"

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

ParameterManager::ParameterManager(
        WireNetwork::Ptr wire_network, Float default_thickness) :
    m_wire_network(wire_network),
    m_thickness_params(wire_network, ParameterCommon::VERTEX, default_thickness),
    m_offset_params(wire_network, ParameterCommon::VERTEX, 0.0) { }

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

VectorF ParameterManager::evaluate_thickness(
        const ParameterManager::Variables& vars) {
    return m_thickness_params.evaluate(vars);
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

