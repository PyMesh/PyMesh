/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "MeshAttributes.h"

#include <sstream>

#include <Core/EigenTypedef.h>
#include <Core/Exception.h>

#include "MeshAttribute.h"
#include "MeshAttributeFactory.h"

using namespace PyMesh;

bool PyMesh::MeshAttributes::has_attribute(const std::string& name) {
    AttributeMap::iterator itr = m_attributes.find(name);
    return (itr != m_attributes.end());
}

void PyMesh::MeshAttributes::add_empty_attribute(const std::string& name) {
    MeshAttribute::Ptr attr = MeshAttributeFactory::create(name);
    m_attributes.insert(AttributeMapEntry(name, attr));
}

void PyMesh::MeshAttributes::add_attribute(const std::string& name, Mesh& mesh) {
    MeshAttribute::Ptr attr = MeshAttributeFactory::create(name);
    attr->compute_from_mesh(mesh);
    m_attributes.insert(AttributeMapEntry(name, attr));
}

void PyMesh::MeshAttributes::remove_attribute(const std::string& name) {
    AttributeMap::iterator itr = m_attributes.find(name);
    if (itr == m_attributes.end()) {
        std::stringstream err_msg;
        err_msg << "Attribute \"" << name << "\" does not exist.";
        throw RuntimeError(err_msg.str());
    }
    m_attributes.erase(itr);
}

VectorF& PyMesh::MeshAttributes::get_attribute(const std::string& name) {
    AttributeMap::iterator itr = m_attributes.find(name);
    if (itr == m_attributes.end()) {
        std::stringstream err_msg;
        err_msg << "Attribute \"" << name << "\" does not exist.";
        throw RuntimeError(err_msg.str());
    }
    return itr->second->get_values();
}

void PyMesh::MeshAttributes::set_attribute(const std::string& name, VectorF& value) {
    MeshAttribute::Ptr attr;
    AttributeMap::iterator itr = m_attributes.find(name);
    if (itr == m_attributes.end()) {
        std::stringstream err_msg;
        err_msg << "Attribute \"" << name << "\" does not exist.";
        throw RuntimeError(err_msg.str());
    } else {
        attr = itr->second;
    }
    attr->set_values(value);
}

MeshAttributes::AttributeNames PyMesh::MeshAttributes::get_attribute_names() const {
    AttributeNames names;
    for (AttributeMap::const_iterator itr = m_attributes.begin();
            itr != m_attributes.end(); itr++) {
        names.push_back(itr->first);
    }
    return names;
}

