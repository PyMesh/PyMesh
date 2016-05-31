/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "WireAttributes.h"

#include <sstream>

#include <Core/Exception.h>
#include <Wires/WireNetwork/WireNetwork.h>

using namespace PyMesh;

namespace WireAttributesHelper {
    void throw_attribute_not_found_error(const std::string& name) {
        std::stringstream err_msg;
        err_msg << "Attribute " << name << " does not exist.";
        throw RuntimeError(err_msg.str());
    }

    void throw_attribute_exists_error(const std::string& name) {
        std::stringstream err_msg;
        err_msg << "Attribute " << name << " already exists.";
        throw RuntimeError(err_msg.str());
    }
}

using namespace WireAttributesHelper;

bool WireAttributes::has_attribute(const std::string& name) const {
    AttrMap::const_iterator itr = m_attr_map.find(name);
    return itr != m_attr_map.end();
}

const MatrixFr& WireAttributes::get_attribute(const std::string& name) const {
    AttrMap::const_iterator itr = m_attr_map.find(name);
    if (itr == m_attr_map.end()) {
        throw_attribute_not_found_error(name);
    }
    return itr->second->get_values();
}

void WireAttributes::add_attribute(const WireNetwork& wire_network,
        const std::string& name, bool vertex_wise, bool auto_compute) {
    AttrMap::const_iterator itr = m_attr_map.find(name);
    if (itr != m_attr_map.end()) {
        throw_attribute_exists_error(name);
    }

    WireAttribute::Ptr attribute = WireAttribute::create(name,
            vertex_wise ? WireAttribute::VERTEX : WireAttribute::EDGE);
    if (auto_compute) attribute->compute(wire_network);

    m_attr_map[name] = attribute;
}

void WireAttributes::set_attribute(const WireNetwork& wire_network,
        const std::string& name, const MatrixFr& values) {
    AttrMap::iterator itr = m_attr_map.find(name);
    if (itr == m_attr_map.end()) {
        throw_attribute_not_found_error(name);
    }

    if (itr->second->get_attribute_type() == WireAttribute::VERTEX) {
        if (values.rows() !=  wire_network.get_num_vertices()) {
            std::stringstream err_msg;
            err_msg << "Vertex attribute has " << values.rows() << " rows" << std::endl;
            err_msg << "Expect " << wire_network.get_num_vertices() << " rows";
            throw RuntimeError(err_msg.str());
        }
    } else {
        if (values.rows() !=  wire_network.get_num_edges()) {
            std::stringstream err_msg;
            err_msg << "Edge attribute has " << values.rows() << " rows" << std::endl;
            err_msg << "Expect " << wire_network.get_num_edges() << " rows";
            throw RuntimeError(err_msg.str());
        }
    }

    itr->second->set_values(values);
}

bool WireAttributes::is_vertex_attribute(const std::string& name) const {
    AttrMap::const_iterator itr = m_attr_map.find(name);
    if (itr == m_attr_map.end()) {
        throw_attribute_not_found_error(name);
    }
    return itr->second->get_attribute_type() == WireAttribute::VERTEX;
}

void WireAttributes::remove_attribute(const std::string& name) {
    AttrMap::iterator itr = m_attr_map.find(name);
    m_attr_map.erase(itr);
}

std::vector<std::string> WireAttributes::get_attribute_names() const {
    std::vector<std::string> names;
    for (auto itr : m_attr_map) {
        names.push_back(itr.first);
    }
    return names;
}

