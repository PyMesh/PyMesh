#include "MeshAttributes.h"

#include <iostream>

#include "MeshAttribute.h"
#include "MeshAttributeFactory.h"
#include "EigenTypedef.h"

bool MeshAttributes::has_attribute(const std::string& name) {
    AttributeMap::iterator itr = m_attributes.find(name);
    return (itr != m_attributes.end());
}

void MeshAttributes::add_empty_attribute(const std::string& name) {
    MeshAttribute::Ptr attr = MeshAttributeFactory::create(name);
    m_attributes.insert(AttributeMapEntry(name, attr));
}

void MeshAttributes::add_attribute(const std::string& name, Mesh& mesh) {
    MeshAttribute::Ptr attr = MeshAttributeFactory::create(name);
    attr->compute_from_mesh(mesh);
    m_attributes.insert(AttributeMapEntry(name, attr));
}

void MeshAttributes::remove_attribute(const std::string& name) {
    AttributeMap::iterator itr = m_attributes.find(name);
    if (itr == m_attributes.end()) {
        std::cerr << "Warning: Attribute \"" << name << "\" does not exist."
            << std::endl;
        return;
    }
    m_attributes.erase(itr);
}

VectorF& MeshAttributes::get_attribute(const std::string& name) {
    AttributeMap::iterator itr = m_attributes.find(name);
    if (itr == m_attributes.end()) {
        std::cerr << "Attribute \"" << name << "\" does not exist yet."
            << std::endl;
    }
    return itr->second->get_values();
}

void MeshAttributes::set_attribute(const std::string& name, VectorF& value) {
    MeshAttribute::Ptr attr;
    AttributeMap::iterator itr = m_attributes.find(name);
    if (itr == m_attributes.end()) {
        std::cerr << "Cannot set non-exist attribute \"" << name << "\""
            << std::endl;
        assert(false);
        return;
    } else {
        attr = itr->second;
    }
    attr->set_values(value);
}

MeshAttributes::AttributeNames MeshAttributes::get_attribute_names() const {
    AttributeNames names;
    for (AttributeMap::const_iterator itr = m_attributes.begin();
            itr != m_attributes.end(); itr++) {
        names.push_back(itr->first);
    }
    return names;
}

