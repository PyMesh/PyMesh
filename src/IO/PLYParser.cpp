/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "PLYParser.h"
#include <algorithm>
#include <sstream>
#include <string>
#include <vector>

#include <Core/EigenTypedef.h>
#include <Core/Exception.h>

#include "rply.h"

using namespace PyMesh;

namespace PLYParserHelper {
    void throw_attribute_not_found_exception(
            const std::string& name) {
        std::stringstream err_msg;
        err_msg << "Attribute " << name << " does not exist";
        throw RuntimeError(err_msg.str());
    }

    void assert_success(bool val) {
        if (!val) {
            throw RuntimeError("PLY parsing failed");
        }
    }

    std::string form_attribute_name(const std::string& elem_name,
            const std::string& prop_name) {
        return elem_name + "_" + prop_name;
    }

    int ply_parser_call_back(p_ply_argument argument) {
        p_ply_element elem;
        p_ply_property prop;
        const char* elem_name;
        const char* prop_name;
        PLYParser* parser;
        long prop_len;
        long value_idx;

        assert_success(ply_get_argument_element(argument, &elem, NULL));
        assert_success(ply_get_argument_property(argument, &prop, &prop_len, &value_idx));
        assert_success(ply_get_element_info(elem, &elem_name, NULL));
        assert_success(ply_get_property_info(prop, &prop_name, NULL, NULL, NULL));
        assert_success(ply_get_argument_user_data(argument, (void**)&parser, NULL));

        Float value = ply_get_argument_value(argument);

        if (value_idx >= 0)
            parser->add_property_value(elem_name, prop_name, value);
        return 1;
    }

    void parse_ply(const std::string& filename, PLYParser* parser) {
        p_ply ply = ply_open(filename.c_str(), NULL, 0, NULL);
        assert_success(ply != NULL);
        assert_success(ply_read_header(ply));

        int r;
        const char* elem_name;
        const char* prop_name;
        long num_elements;
        p_ply_element element = ply_get_next_element(ply, NULL);
        while (element != NULL) {
            assert_success(ply_get_element_info(element, &elem_name, &num_elements));

            p_ply_property property = ply_get_next_property(element, NULL);
            while (property != NULL) {
                assert_success(ply_get_property_info(property, &prop_name, NULL, NULL, NULL));

                ply_set_read_cb(ply, elem_name, prop_name, ply_parser_call_back, parser, 0);
                parser->add_property(elem_name, prop_name, num_elements);

                property = ply_get_next_property(element, property);
            }
            element = ply_get_next_element(ply, element);
        }
        assert_success(ply_read(ply));
        ply_close(ply);
    }
}

using namespace PLYParserHelper;

bool PLYParser::parse(const std::string& filename) {
    parse_ply(filename, this);
    init_vertices();
    init_faces();
    init_voxels();
    return true;
}

size_t PLYParser::num_vertices() const {
    return m_num_vertices;
}

size_t PLYParser::num_faces() const {
    return m_num_faces;
}

size_t PLYParser::num_voxels() const {
    return m_num_voxels;
}

size_t PLYParser::num_attributes() const {
    return m_attributes.size();
}

PLYParser::AttrNames PLYParser::get_attribute_names() const {
    AttrNames names;
    for (AttributeMap::const_iterator itr = m_attributes.begin();
            itr != m_attributes.end(); itr++) {
        names.push_back(itr->first);
    }
    return names;
}

size_t PLYParser::get_attribute_size(const std::string& name) const {
    AttributeMap::const_iterator itr = m_attributes.find(name);
    if (itr == m_attributes.end()) {
        throw_attribute_not_found_exception(name);
    }
    return itr->second.size();
}

void PLYParser::export_vertices(Float* buffer) {
    std::copy(m_vertices.data(), m_vertices.data() + m_vertices.size(), buffer);
}

void PLYParser::export_faces(int* buffer) {
    std::copy(m_faces.data(), m_faces.data() + m_faces.size(), buffer);
}

void PLYParser::export_voxels(int* buffer) {
    std::copy(m_voxels.data(), m_voxels.data() + m_voxels.size(), buffer);
}

void PLYParser::export_attribute(const std::string& name, Float* buffer) {
    AttributeMap::const_iterator itr = m_attributes.find(name);
    if (itr == m_attributes.end()) {
        throw_attribute_not_found_exception(name);
    }
    const std::vector<Float>& attr = itr->second;
    std::copy(attr.begin(), attr.end(), buffer);
}

void PLYParser::add_property(const std::string& elem_name,
        const std::string& prop_name, size_t size) {
    if (elem_name == "vertex") {
        m_num_vertices = size;
    } else if (elem_name == "face") {
        m_num_faces = size;
    } else if (elem_name == "voxel") {
        m_num_voxels = size;
    }

    std::string attr_name = form_attribute_name(elem_name, prop_name);
    AttributeMap::const_iterator itr = m_attributes.find(attr_name);
    if (itr == m_attributes.end()) {
        m_attributes[attr_name] = std::vector<Float>();
    } else {
        std::stringstream err_msg;
        err_msg << "Duplicated property name: " << prop_name << std::endl;
        err_msg << "PyMesh requires unique custom property names";
        throw IOError(err_msg.str());
    }
}

void PLYParser::add_property_value(const std::string& elem_name,
        const std::string& prop_name, Float value) {
    std::string attr_name = form_attribute_name(elem_name, prop_name);
    AttributeMap::iterator itr = m_attributes.find(attr_name);
    if (itr == m_attributes.end()) {
        throw_attribute_not_found_exception(attr_name);
    }
    std::vector<Float>& attr = itr->second;
    attr.push_back(value);
}

void PLYParser::init_vertices() {
    std::string field_names[] = {
        std::string("vertex_x"),
        std::string("vertex_y"),
        std::string("vertex_z") };

    AttributeMap::const_iterator iterators[3];
    m_dim = 0;
    for (size_t i=0; i<3; i++) {
        iterators[i] = m_attributes.find(field_names[i]);
        if (iterators[i] != m_attributes.end()) {
            m_dim++;
        }
    }

    m_vertices = VectorF(m_dim * m_num_vertices);
    for (size_t i=0; i<m_dim; i++) {
        const std::vector<Float>& coord = iterators[i]->second;
        assert(coord.size() == m_num_vertices);
        for (size_t j=0; j<m_num_vertices; j++) {
            m_vertices[j*m_dim + i] = coord[j];
        }
    }
    if (!m_vertices.allFinite()) {
        throw IOError("NaN or Inf detected in input file.");
    }
}

void PLYParser::init_faces() {
    AttributeMap::const_iterator face_attr_itr = m_attributes.find("face_vertex_indices");
    if (face_attr_itr == m_attributes.end()) {
        face_attr_itr = m_attributes.find("face_vertex_index");
        if (face_attr_itr == m_attributes.end()) {
            m_vertex_per_face = 3; // default to triangle.
            m_num_faces = 0;
            return;
        }
    }
    const std::vector<Float>& faces = face_attr_itr->second;
    assert(faces.size() == 0 || faces.size() % m_num_faces == 0);
    m_vertex_per_face = m_num_faces == 0 ? 3:faces.size() / m_num_faces;
    m_faces = VectorI(faces.size());
    std::copy(faces.begin(), faces.end(), m_faces.data());
}

void PLYParser::init_voxels() {
    AttributeMap::const_iterator voxel_attr_itr = m_attributes.find("voxel_vertex_indices");
    if (voxel_attr_itr == m_attributes.end()) {
        voxel_attr_itr = m_attributes.find("voxel_vertex_index");
        if (voxel_attr_itr == m_attributes.end()) {
            m_vertex_per_voxel = 0;
            return;
        }
    }
    const std::vector<Float>& voxels = voxel_attr_itr->second;
    assert(voxels.size() % m_num_voxels == 0);
    m_vertex_per_voxel = voxels.size() / m_num_voxels;
    m_voxels = VectorI(voxels.size());
    std::copy(voxels.begin(), voxels.end(), m_voxels.data());
}

