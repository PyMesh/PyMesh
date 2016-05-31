/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <fstream>
#include <map>
#include <string>
#include <vector>

#include <Core/EigenTypedef.h>

namespace PyMesh {

class MshLoader {
    public:
        typedef std::map<std::string, VectorF> FieldMap;
        typedef std::vector<std::string> FieldNames;

    public:
        MshLoader(const std::string& filename);

    public:
        const VectorF& get_nodes() const { return m_nodes; }
        const VectorI& get_elements() const { return m_elements; }

        VectorF& get_node_field(const std::string& fieldname) {
            return m_node_fields[fieldname];
        }

        VectorF& get_element_field(const std::string& fieldname) {
            return m_element_fields[fieldname];
        }

        bool is_node_field(const std::string& fieldname) {
            return (m_node_fields.find(fieldname) != m_node_fields.end());
        }

        bool is_element_field(const std::string& fieldname) {
            return (m_element_fields.find(fieldname) != m_element_fields.end());
        }

        FieldNames get_node_field_names() const;
        FieldNames get_element_field_names() const;

        size_t get_nodes_per_element() const {
            return m_nodes_per_element;
        }

        size_t get_element_type() const {
            return m_element_type;
        }


    public:
        enum ErrorCode {
            INVALID_FORMAT,
            NOT_IMPLEMENTED
        };

    private:
        void parse_nodes(std::ifstream& fin);
        void parse_elements(std::ifstream& fin);
        void parse_node_field(std::ifstream& fin);
        void parse_element_field(std::ifstream& fin);
        void parse_unknown_field(std::ifstream& fin,
                const std::string& fieldname);

        void eat_white_space(std::ifstream& fin);
        int num_nodes_per_elem_type(int elem_type);

    private:
        bool m_binary;
        size_t m_data_size;
        size_t m_nodes_per_element;
        size_t m_element_type;
        VectorF m_nodes;
        VectorI m_elements;
        FieldMap m_node_fields;
        FieldMap m_element_fields;
};

}
